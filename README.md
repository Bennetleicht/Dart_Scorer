Arduino Dart Scorer
Ein hardwarebasierter, automatisierter Punktezähler für Dartspiele auf Basis eines Arduino UNO R3. 
Entwickelt als robustere und kostengünstigere Alternative zu empfindlichen Smartphone- oder Tablet-Apps direkt neben der Dartscheibe.

🚀 FeaturesSpielmodi: 
Unterstützt die klassischen Modi 301 und 501. 

Eingabe & Validierung: Punkteeingabe über ein 4x4 Tastenfeld (Membrane Switch Module) mit automatischer Subtraktion und Prüfung auf Fehlwürfe oder Überwerfen (Bust).  

LCD-Display: Live-Anzeige des aktuellen Punktestands und von Statusmeldungen (z. B. "Invalid Input", "Bust", "Congrats You Won").  

Sound-Feedback: Ein passiver Buzzer spielt dynamische Tonfolgen für verschiedene Events ab (z. B. normaler Wurf, ungültige Eingabe, Sieg oder ein spezieller Sound bei einer 180).  

Steuerung: Einfache Bedienung (# zum Bestätigen, * zum Löschen, D für einen jederzeitigen Reset).  

🛠️ Hardware-Komponenten:
Arduino UNO R3   

LCD1602 Display-Modul   

4x4 Membrane Matrix Keypad  

Passiver Buzzer   

10kΩ Potentiometer (für den Display-Kontrast)   
