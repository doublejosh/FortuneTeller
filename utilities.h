#include "configs.h"

/**
 * Simple text output (refactor in progress).
 **/
void printDebug (String msg) {
    if (DEBUG) {
        Serial.println(msg);
    }
}

/**
 * Wrap text to display, clearly pasted from StackOverflow :)
 */

void wrapTxtToScreen (LiquidCrystal &lcd, String msg) {
  	byte offset = 0, charNum = 0, dataLength = strlen(msg.c_str());

  	if (dataLength > WIDTH * HEIGHT) {
		dataLength = WIDTH * HEIGHT + 3;
	}
	for (byte rowNum = 0; rowNum < HEIGHT; rowNum++) {
		lcd.setCursor(0, rowNum);
		charNum = rowNum * WIDTH + offset;
		while (charNum < ((rowNum + 1) *WIDTH) + offset) {
			if ((charNum - offset == rowNum * WIDTH) && (charNum < dataLength-offset) && (msg[charNum] == ' ')) {
				charNum++;
				offset++;
			}
			if (charNum - offset >= dataLength-offset) {
				lcd.write(254);
			}
			//if (charNum-offset >= dataLength-offset) {
			// 	lcd.print("x");
			// }
			else if (charNum - offset < dataLength-offset) {
				lcd.print(msg[charNum]);
			}
			charNum++;
		}
	}
};

/**
 * Debug Firebase client data objects.
 */
void printResult(FirebaseData &data) {
    if (data.dataType() == "int")
        Serial.println(data.intData());
    else if (data.dataType() == "float")
        Serial.println(data.floatData(), 5);
    else if (data.dataType() == "double")
        printf("%.9lf\n", data.doubleData());
    else if (data.dataType() == "boolean")
        Serial.println(data.boolData() == 1 ? "true" : "false");
    else if (data.dataType() == "string")
        Serial.println(data.stringData());
    else if (data.dataType() == "json") {
        Serial.println();
        FirebaseJson &json = data.jsonObject();
        //Print all object data
        Serial.println("Pretty printed JSON data:");
        String jsonStr;
        json.toString(jsonStr,true);
        Serial.println(jsonStr);
        Serial.println();
        Serial.println("Iterate JSON data:");
        Serial.println();
        size_t len = json.iteratorBegin();
        String key, value = "";
        int type = 0;
        for (size_t i = 0; i < len; i++) {
            json.iteratorGet(i, type, key, value);
            Serial.print(i);
            Serial.print(", ");
            Serial.print("Type: ");
            Serial.print(type == JSON_OBJECT ? "object" : "array");
            if (type == JSON_OBJECT) {
                Serial.print(", Key: ");
                Serial.print(key);
            }
            Serial.print(", Value: ");
            Serial.println(value);
        }
        json.iteratorEnd();
    }
    else if (data.dataType() == "array") {
        Serial.println();
        //get array data from FirebaseData using FirebaseJsonArray object
        FirebaseJsonArray &arr = data.jsonArray();
        //Print all array values
        Serial.println("Pretty printed Array:");
        String arrStr;
        arr.toString(arrStr,true);
        Serial.println(arrStr);
        Serial.println();
        Serial.println("Iterate array values:");
        Serial.println();
        for (size_t i = 0; i < arr.size(); i++) {
            Serial.print(i);
            Serial.print(", Value: ");

            FirebaseJsonData &jsonData = data.jsonData();
            //Get the result data from FirebaseJsonArray object
            arr.get(jsonData, i);
            if (jsonData.typeNum == JSON_BOOL)
                Serial.println(jsonData.boolValue ? "true" : "false");
            else if (jsonData.typeNum == JSON_INT)
                Serial.println(jsonData.intValue);
            else if (jsonData.typeNum == JSON_DOUBLE)
                printf("%.9lf\n", jsonData.doubleValue);
            else if (jsonData.typeNum == JSON_STRING ||
                     jsonData.typeNum == JSON_NULL ||
                     jsonData.typeNum == JSON_OBJECT ||
                     jsonData.typeNum == JSON_ARRAY)
                Serial.println(jsonData.stringValue);
        }
    }
}
