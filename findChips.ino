/***************************************************
  eightPositionTempMonitor - findChips.ino
  Modified by Jim Mayhugh
  07/16/2021
*****************************************************/

void findChips(void)
{
  uint8_t i;
  chipCnt=0;
  while(chipCnt < maxDS)
  {
    if(setDebug & findChipsDebug)
    {
      Serial.print("chipCnt = ");
      Serial.print(chipCnt);
      Serial.println();
    }
    if ( !ds[chipCnt]->search(ds18b20[chipCnt].tempAddress)) {
      if(setDebug & findChipsDebug)
      {
        Serial.println("No more addresses.");
        Serial.println();
      }
      ds[chipCnt]->reset_search();
      delay(250);
      chipCnt++;
      continue;
    }

    if(setDebug & findChipsDebug)
    {
      Serial.print("ROM =");
      for( i = 0; i < 8; i++)
      {
        Serial.write(' ');
        if(ds18b20[chipCnt].tempAddress[i] < 16)
          Serial.print('0');
        Serial.print(ds18b20[chipCnt].tempAddress[i], HEX);
      }
    }


    if (ds[chipCnt]->crc8(ds18b20[chipCnt].tempAddress, 7) != ds18b20[chipCnt].tempAddress[7])
    {
      if(setDebug & findChipsDebug)
      {
        Serial.println(" CRC is not valid!");
        chipCnt++;
        continue;
      }
    }else if(setDebug & findChipsDebug){
        Serial.println(" CRC OK");
    }

    if(setDebug & findChipsDebug)
    {
      Serial.println("Reset ds");
    }
    ds[chipCnt]->reset();

    if(setDebug & findChipsDebug)
    {
      Serial.println("Select");
    }
    ds[chipCnt]->select(ds18b20[chipCnt].tempAddress);
    
    if(setDebug & findChipsDebug)
    {
      Serial.println("Write To Scratchpad");
    }
    ds[chipCnt]->write(0x4E); // write to scratchpad ALL THREE BYTES MUST BE WRITTEN
    ds[chipCnt]->write(0x00); // low alarm -  1st byte
    ds[chipCnt]->write(0x00); // high alarm - 2nd byte
    
    if(setDebug & findChipsDebug)
    {
      Serial.println("Set 12 bit accuracy");
    }
    ds[chipCnt]->write(0x7F); // configuration register - 3rd buye - 12 bit accuracy (0.5deg C)
    delay(5);

    if(setDebug & findChipsDebug)
    {
      Serial.println("Reset ds");
      Serial.println();
    }

    ds[chipCnt]->reset();
    chipCnt++;
  }
}
