/***************************************************
  eightPositionTempMonitor - startTemp.ino
  Modified by Jim Mayhugh
  07/16/2021
*****************************************************/
void startTemp(void)
{
uint8_t i;
chipCnt=0;
  while(chipCnt<maxDS)
  {
    if(setDebug & findChipsDebug)
    {
      Serial.print("Chip");
      Serial.print(chipCnt);
      Serial.print(" Address:");

      for(i=0;i<chipAddrSize;i++)
      {
        if(ds18b20[chipCnt].tempAddress[i] < 0x10)
          Serial.print(" 0");
        else
          Serial.print(' ');
        Serial.print(ds18b20[chipCnt].tempAddress[i], HEX);
      }
      Serial.print(" CRC=");
      Serial.print( ds[chipCnt]->crc8(ds18b20[chipCnt].tempAddress, 7), HEX);
      Serial.println();
   }
    ds[chipCnt]->reset();
    ds[chipCnt]->select(ds18b20[chipCnt].tempAddress);
    ds[chipCnt]->write(0x44,1);         // start conversion, with parasite power on at the end
    chipCnt++;
  }
    if(setDebug & findChipsDebug)
      Serial.println();
}

void readTemp(void)
{
chipCnt=0;
uint8_t i;
uint8_t present;

  while(chipCnt<maxDS)
  {
    if(setDebug & findChipsDebug)
    {
      Serial.print("Chip");
      Serial.print(chipCnt);
      Serial.print(" Data:");
    }

    present = ds[chipCnt]->reset();
    ds[chipCnt]->select(ds18b20[chipCnt].tempAddress);    
    ds[chipCnt]->write(0xBE);         // Read Scratchpad

    if(setDebug & findChipsDebug)
    {
      Serial.print("P=");
      Serial.print(present,HEX);
      Serial.print(" ");
     }
    if(present == 1)
    {
      for ( i = 0; i < tempDataSize; i++)          // we need 9 bytes
      { 
        ds18b20[chipCnt].tempData[i] = ds[chipCnt]->read();
        if(setDebug & findChipsDebug)
        {
          if(ds18b20[chipCnt].tempData[i] < 0x10)
            Serial.print(" 0");
          else
            Serial.print(' ');
          Serial.print(ds18b20[chipCnt].tempData[i], HEX);
        }
      }
      if(setDebug & findChipsDebug)
      {
        Serial.print(" CRC=");
        Serial.print( ds[chipCnt]->crc8(ds18b20[chipCnt].tempData, 8), HEX);
      }
      uint32_t raw = (ds18b20[chipCnt].tempData[1] << 8) | ds18b20[chipCnt].tempData[0];
      float rawFloat = (float) raw;
      ds18b20[chipCnt].tempC = rawFloat/16;
      
      if(setDebug & findChipsDebug)
      {
        Serial.print(" Temp= ");
        Serial.print(ds18b20[chipCnt].tempC,2);
        Serial.println();
      }
      
      chipCnt++;
    }else{
      if(setDebug & findChipsDebug)
        Serial.println();
      chipCnt++;
      continue;
    }
  }    
}
