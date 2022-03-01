
const int SENSOR_PIN = A0;       

int currentDay; //used to store the current day (1-31)

int timeInSun; //used to store the amount of time in the sun, in seconds

boolean inSun; //Used to tell if the sensor is in the sun or not

unsigned int lightValue; //Used to store the current light value
char timeString[35]; //Used to send the time in the sun in string form to IFTTT


void setup()
{

    currentDay = Time.day(); //set the current day
    timeInSun = 0; // set the time in the sun today to 0
    inSun = false;
    lightValue = 0;
}

void loop() {
    
    //Check to see if the current day has changed
    if (hasDayChanged()){
        //if the day has changed reset values
        currentDay = Time.day();
        timeInSun = 0;
        inSun = false;
        lightValue = 0;
    }
    
    
    //get Light value
    lightValue = getLightValue();
    
    Serial.println(lightValue); //Used for debugging
    Serial.println(timeInSun); //Used for debugging
    
    //check if the unit is in the sun
    if (isInSun(lightValue)){
        //If it is in the sun
        
        //If it wasn't already in the sun, send an alert, saying it has been introduced to the sun and how much time today it's been in the sun
        if(!inSun){
            sendEnteredLight();
        }
        
        
        //Save inSun to be true, so we know its not just been introduced to the sun
        inSun = true;
        
        //If the light is in the sun, increment the time in sun by 1 second
        timeInSun+=1;
        
        //How much time has it spent in the sun? If more than 2 Hours, send a warning
        if (timeInSun >= 7200){
            
            sendWarning();
            
        }
        
    } else if (!isInSun(lightValue)){
        
        //Its not in the sun
        
        //Check if it just left the sun
        
        if(inSun){
            //If so notify it left the light
            sendLeftLight();
        }
    
        inSun = false;
        
    }


    // Wait 1 second
    delay(1s);

}

void createTimeString(){
    
    int hours, mins, secs;
	
    hours = (timeInSun/3600); 
	
	mins = (timeInSun -(3600*hours))/60;
	
	secs = (timeInSun -(3600*hours)-(mins*60));

	sprintf(timeString, "%d Hours, %d Minutes, %d Seconds", hours, mins, secs);
	
	Serial.println(timeString);
    
}

void sendWarning(){
    
    //IFTTT Event named "Warning"
    createTimeString();
    bool success =  Particle.publish("light_warning", String(timeString), WITH_ACK);
    Serial.println(success);
    delay(299s); //Wait 5 Mins
    
}

void sendLeftLight(){
    Serial.println("Left Light");
     //IFTTT Event named "Left Light"
    createTimeString();
    bool success =  Particle.publish("light_left", String(timeString), WITH_ACK);
    Serial.println(success);
    delay(1s);
    
}

void sendEnteredLight(){
     Serial.println("Entered Light");
    //IFTTT Event named "Entered Light"
    createTimeString();
    //bool success =  Particle.publish("entered", String(timeString), WITH_ACK);
    bool success =  Particle.publish("light_entered", String(timeString));
    Serial.println(success);
    timeInSun+=1;
    delay(1s);
    
}

boolean hasDayChanged(){
    //Return true if the day has changed
    if (currentDay != Time.day()){
        return true;
    }
    return false;
}

boolean isInSun(int lightReading) {
    //If the light value is > 3500 it is in the sunlight
    if (lightReading > 3500){
        return true;
    } 
    return false;
}

unsigned int getLightValue(){
    
    lightValue = analogRead(SENSOR_PIN);
    
    return lightValue;
    
}