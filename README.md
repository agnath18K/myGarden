# myGarden
/*                     myGarden
 *            Created by agnath18@gmail.com
 *  GPIO PINS
 *  PUMP               : D1
 *  BUZZER             : D2
 *  Water Level Sensor : D5
 *  Status LED         : D6
 *  NTP OFFSET         : 19800
 * 
 *            MONITOR
 *  Current_Level : Current Water Level
 *  Duration : Duration for activating PUMP
 *  Last_Status : Last Successful Watering Time
 *  Tank_Status : Water Level Of Tank ("Normal" OR "Low Level")
 *  Threshold_Level : Minimum Water Level Required
 *  Delay_Time : Interval For Updating Values
 *  Note : Delay Time Over 60Sec may ignore Scheduled Triggers
 *  
 *           SCHEDULE
 *  Time1 : Set Watering Time1
 *  Time2 : Set Watering Time2
 *  
 *             CMD
 *  Terminal
 *  -> "restart" For Reset
 *  ->  "ota"    For Over The Air (OTA) code updation.
 *  Note :   Start the OTA only after reciveing confirmation in the cmd/status.
 */
