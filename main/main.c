#include <stdio.h>
#include "Int_BDR6120.h"
void app_main(void)
{
   //1.初始化一次
   Int_BDR6120_Init();

   //2.开门
   Int_BDR6120_Open();
}
