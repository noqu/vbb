
/*------------------------ U_INIT_GLOB -------------------------*/
/* Initialisiert die Datenstruktur 'u_glob', die das zu loesende 
 * Problem vollstaendig beschreibt. Diese Daten werden an alle 
 * problemspezifischen Funktionen weitergegeben und dienen zur 
 * parametrischen Beschreibung von BB-Knoten. 
 * Zur Eingabe koennen Funktionen aus 'read.c' verwendet werden.
 * Da diese Funktion nur auf dem Root-Transputer eingesetzt werden
 * kann, duerfen Fehler direkt auf Standardausgabe gemeldet werden. 
 * Der Rueckgabewert ist OK oder ein problemspezifischer Statuswert 
 * bei Fehlern. 
 */
/*------------------------ U_LOWER_BOUND ------------------------*/
/* Liefert die untere Schranke des Zielfunktionswertes fuer den 
 * Knoten 'node'. Diese Schranke wird nach einem 
 * problemspezifischen Verfahren berechnet.  Es ist 
 * zulaessig, dass 'node' beim Aufruf der Funktion veraendert wird. 
 * (Anm: Da die Funktion u.U. mehrmals fuer denselben Knoten 
 * aufgerufen wird, ist es zweckmaessig, den Wert der unteren 
 * Schranke im Knoten abzulegen und bei erneuten Aufrufen direkt 
 * zurueckzugeben.)
 */
/*---------------------------- U_PRIORITY -----------------------*/
/* Berechnet die Prioritaet des Knotens node. Hier wird fuer alle
 * Instanzen die in der Arbeit beschriebene parametrische 
 * Prioritaetsfunktion verwendet. Fuer einen Parameter, der kleiner
 * als 0 ist, kann zusaetzlich reine Tiefensuche verlangt werden.
 * Diese Eigenschaft dient zu Testzwecken und ist im Text der Arbeit
 * nicht dokumentiert.
 */
/*------------------------ U_UNFLAT_GLOB -----------------------*/
/* Erzeugt aus dem Inhalt des flachen Puffers flat den Inhalt der
 * Datenstruktur u_glob. U_glob wird dabei angelegt. Beim Anlegen von
 * flat muss in geeigneter Weise die Groesse mitgeliefert werden. 
 */ 
/*------------------------ U_TRACE_NODE -------------------------*/
/* Gibt den Inhalt eines u_nodes in einen String aus, der dann als
 * Trace-Information ausgegeben werden kann. Wird von den
 * Funktionen in trace.c aufgerufen, wenn Tracing eingeschaltet ist.
 */
