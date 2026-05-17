# Ergebnisse Prio-Config (Iteration 3)

## Zielsetzung
Die Priorisierungsparameter sollen die Anforderungen bestmoeglich erfuellen:
1) moeglichst wenig Crashes,
2) moeglichst kleine Wartezeiten,
3) moeglichst gleiche Queue-Laengen.

## Gewaehlte Parameter (Iteration 3)
- fuel_gap_weight: 3
- size_balance_weight: 60
- fuel_risk_weight: 10
- tie_breaker_queue: 1

## Vorgehen (kurz)
- Drei Iterationen wurden manuell getestet.
- Pro Iteration wurden drei Runs ausgefuehrt und die Kennzahlen verglichen.
- Iteration 3 wurde als Finalwert gewaehlt, weil sie gute Wartezeiten bietet und die Queue-Differenz im Mittel klein bleibt, bei weiterhin 0 Crashes.

## Kennzahlen (Finaler Lauf, Iteration 3)
- avg_wait_landing: 3.05
- avg_wait_takeoff: 0.00
- total_crashes: 0
- avg_queue_diff (mittlere Differenz der beiden Landing-Queues): 0.83

## Erklaerung der Wirkung
- Das relativ hohe size_balance_weight (60) haelt die beiden Landing-Queues nahe beieinander, ohne die Warteschlangen strikt zu erzwingen.
- Ein moderates fuel_gap_weight (3) reduziert starke Schwankungen in der Reihenfolge, verhindert aber keine Balance.
- Der fuel_risk_weight (10) sorgt dafuer, dass Flieger mit kritischer Reichweite bevorzugt werden, um Crashes zu vermeiden.

## Fazit
Die finale Prio-Config in Iteration 3 liefert im aktuellen Setup:
- keine Crashes (Ziel 1 erreicht),
- stabile Wartezeiten im niedrigen Bereich (Ziel 2 gut),
- geringe Differenz zwischen den Landing-Queues (Ziel 3 gut).

Damit ist die Konfiguration geeignet, um sie in der Praesentation als ausbalancierte Loesung zwischen Sicherheit, Effizienz und Fairness zu begruenden.

