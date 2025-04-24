# Arduino_VFC_parser
A parser to map Arduino code into the VFC tools


# 🌀 Arduino Parser INO to VFC IDE  

![image](https://github.com/user-attachments/assets/c41e89d7-eb4d-41ce-a68b-e67ffef2dd65)

Created in just a few hours, this preliminary parser helps safeguard your Arduino sketches by making backups and enhancing readability.  

The backup file is: `_filename.ino`  

## 📌 Overview  

Designed for **Arduino-formatted indentation**, this tool aims to improve code organization and clarity.  

### Features  
- A **Python-based parser** tailored for Arduino sketches  
- Inserts `#begin...` / `#end...` comments (`#endfunc`, `#endif`, `#endloop`, etc.)  
- Makes block structures explicitly visible—beyond just indentation  

## 🔁 3-Phase Parsing of Arduino Files  

1. **Format the file**  
2. **Add structure tags**  
3. **Map structure comments to VFC tokens**  

Once annotated, the file is fed into **VFCode**, a visual flow graph renderer. This reveals the full logical structure in a **visual, explorable format** that integrates with the **Arduino IDE**.  

## 📊 Example  

### Before  
```
void loop() {
    if (sensorReading > threshold) {
        activateMotor();
    }
}
```
```
### After
void loop() { #beginloop
    if (sensorReading > threshold) { #beginif
        activateMotor();
    } #endif
} #endloop
```

