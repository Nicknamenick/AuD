
#!/usr/bin/env python3
"""
(made with AI assistance)
Beispiel: Starte eine .exe / ein Programm und verarbeite dessen Ausgabe.

Usage examples:
python eval/main.py
python eval/main.py --mode streaming --exe ping --args "-n 4 127.0.0.1"
python eval/main.py --mode blocking --exe "C:\\path\\to\\app.exe" --args "--option value"
"""

import argparse
import asyncio
import shlex
import subprocess
import sys
import time
import pathlib
import shutil
import re


def process_line(line: str) -> None:
	# Einfaches Verarbeitungs-Beispiel: Ausgabe mit Prefix.
	print(f"[CHILD] {line}")


class StatsCollector:
	"""Sammelt Crash-Informationen aus Child-Logzeilen.

	- Sucht zuerst nach einer abschließenden Zeile wie "Total crashes: N".
	- Falls nicht vorhanden, summiert alle "CRASHES LAST TICK: X" Werte ("none" -> 0).
	"""
	def __init__(self):
		self._re_tick = re.compile(r"CRASHES LAST TICK:\s*(none|\d+)", re.IGNORECASE)
		self._re_total = re.compile(r"Total crashes:\s*(\d+)", re.IGNORECASE)
		self._got_total = False
		self._total = 0

	def process_line(self, line: str) -> None:
		if not line:
			return
		m = self._re_total.search(line)
		if m:
			try:
				self._total = int(m.group(1))
				self._got_total = True
			except Exception:
				pass
			return

		m2 = self._re_tick.search(line)
		if m2:
			val = m2.group(1)
			try:
				v = 0 if val.lower() == "none" else int(val)
			except Exception:
				v = 0
			if not self._got_total:
				self._total += v

	def finalize(self) -> int:
		return int(self._total)


def run_blocking(exe: str, args=None, timeout=None, stats: StatsCollector = None) -> int:
	cmd = [exe] + (args or [])
	try:
		res = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
	except subprocess.TimeoutExpired:
		print("Process timed out", file=sys.stderr)
		return -1
	stdout = res.stdout or ""
	for line in stdout.splitlines():
		if stats:
			stats.process_line(line)
		process_line(line)
	stderr = res.stderr or ""
	if stderr:
		for line in stderr.splitlines():
			if stats:
				stats.process_line(line)
			print(f"[CHILD-ERR] {line}", file=sys.stderr)
	return res.returncode


def run_streaming(exe: str, args=None, stats: StatsCollector = None) -> int:
	cmd = [exe] + (args or [])
	proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1)
	try:
		if proc.stdout is None:
			return proc.wait()
		for raw in proc.stdout:
			line = raw.rstrip("\r\n")
			if stats:
				stats.process_line(line)
			process_line(line)
	except Exception as e:
		print("Error reading child output:", e, file=sys.stderr)
	finally:
		try:
			proc.stdout.close()
		except Exception:
			pass
		return proc.wait()


async def run_async(exe: str, args=None, stats: StatsCollector = None) -> int:
	args = args or []
	proc = await asyncio.create_subprocess_exec(exe, *args, stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.STDOUT)
	try:
		while True:
			raw = await proc.stdout.readline()
			if not raw:
				break
			if isinstance(raw, bytes):
				text = raw.decode(errors="replace")
			else:
				text = raw
			line = text.rstrip("\r\n")
			if stats:
				stats.process_line(line)
			process_line(line)
	finally:
		return await proc.wait()


def main():
	parser = argparse.ArgumentParser(description="Start an .exe and process its stdout in Python")
	parser.add_argument("--mode", choices=("blocking", "streaming", "async"), default="streaming")
	parser.add_argument("--exe", help="Path to executable (path or name in PATH). If omitted, tries build/sim.exe then a demo child.", default=None)
	parser.add_argument("--args", help="Arguments for the executable (quoted string, will be split)", default="")
	parser.add_argument("--timeout", type=float, help="Timeout for blocking mode (seconds)", default=None)
	parser.add_argument("--repeat", type=int, help="Number of times to run the program and average crashes (default: 50)", default=200)
	ns = parser.parse_args()

	script_dir = pathlib.Path(__file__).resolve().parent

	exe = None
	args = shlex.split(ns.args) if ns.args else []

	if ns.exe:
		# User provided something in --exe
		candidate = pathlib.Path(ns.exe)
		if candidate.is_file():
			exe = str(candidate)
		else:
			# Maybe it's a command in PATH
			if shutil.which(str(ns.exe)):
				exe = str(ns.exe)
			else:
				# Try resolving relative to repo (script_dir parent)
				rel = script_dir.parent / ns.exe
				if rel.is_file():
					exe = str(rel)
				else:
					print(f"Executable not found: {ns.exe}", file=sys.stderr)
					print(f"Tried absolute/path: {candidate}", file=sys.stderr)
					print(f"Tried relative to script: {rel}", file=sys.stderr)
					print(f"Current working directory: {pathlib.Path.cwd()}", file=sys.stderr)
					print("Pass an absolute path or ensure the program is on PATH.", file=sys.stderr)
					sys.exit(2)
	else:
		# No --exe: try default build location next to repo root
		default_candidate = script_dir.parent / "cmake-build-debug" / "sim.exe"
		if default_candidate.is_file():
			exe = str(default_candidate)
		else:
			# Fallback: demo child using current Python
			print("No --exe specified and build not found; running demo child.", file=sys.stderr)
			exe = sys.executable
			args = ["-c", "import time; print('child start'); time.sleep(0.1); print('child end')"]

	repeat = max(1, int(ns.repeat))
	crash_counts = []
	last_rc = 0

	for i in range(repeat):
		print(f"=== Run {i+1}/{repeat} ===")
		stats = StatsCollector()
		if ns.mode == "blocking":
			rc = run_blocking(exe, args, timeout=ns.timeout, stats=stats)
		elif ns.mode == "streaming":
			rc = run_streaming(exe, args, stats=stats)
		else:
			rc = asyncio.run(run_async(exe, args, stats=stats))

		crashed = stats.finalize()
		print(f"Run {i+1} crashes: {crashed}")
		crash_counts.append(crashed)
		last_rc = rc
		# small pause to avoid hammering resources
		time.sleep(0.05)

	avg = (sum(crash_counts) / len(crash_counts)) if crash_counts else 0.0
	print("\n=== Summary ===")
	print(f"Runs: {len(crash_counts)}")
	print(f"Crash counts per run: {crash_counts}")
	print(f"Average crashes per run: {avg:.2f}")

	sys.exit(last_rc if isinstance(last_rc, int) else 0)


if __name__ == "__main__":
	main()
