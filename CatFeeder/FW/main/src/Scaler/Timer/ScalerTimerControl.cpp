// #include "ScalerTimerControl.h"

// void ScalerTimerSetEvent(void (*onTimerEvent)(void))
// {

//     timer1_attachInterrupt(onTimerEvent);
//     timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
//     timer1_write(500000); // 100ms
// }

// // void ICACHE_RAM_ATTR onTimerISR()
// // {
// uint16_t counter = 0;

// //     counter++;
// //     if (counter >= 10)
// //     {
// //         counter = 0;
// //         digitalWrite(R_LED, !(digitalRead(R_LED))); // Toggle LED Pin
// //     }
// //     timer1_write(500000); // 10ms
// // }

// void ScalerTimerReactivate()
// {
//     timer1_write(500000); // 10ms
// }