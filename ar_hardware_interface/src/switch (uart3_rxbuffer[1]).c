           switch (uart3_rxbuffer[1])
			{
			case 0xFD:     //位置模式


				if (uuart3_rxbuffer[2] == 0x02)
				{
	
						if (uart2_txbuffer[0]==0x50)
						{
							uint16_t vel = 0;

							clk = (uint32_t)((uint16_t)uart2_txbuffer[3*currentIndex+2] << 8 |
											 (uint16_t)uart2_txbuffer[3*currentIndex+3] << 0);
							clk = clk *gear_ratio[currentIndex];
							vel = ((uint16_t)(uint16_t)uart2_txbuffer[3*currentIndex+4] << 8 |
											 (uint16_t)uart2_txbuffer[3*currentIndex+5] << 0);
							vel = vel *gear_ratio[currentIndex];

							Emm_V5_Pos_Control(currentIndex+1, uart3_rxbuffer[3*currentIndex+1], vel, 1, clk, 1, 1);
						}
					}
					else
					{
						HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
						currentIndex = 0;
						Emm_V5_Synchronous_motion(0);
					}
				}

				break;

			case 0x43:  //读取系统参数

				if( uart3_rxbuffer[2] == 0x1F && uart3_rxbuffer[3] == 0x09)
				{
					HAL_UART_Transmit_DMA(&huart2, uart3_rxbuffer, Size);
					uint32_t tpos = 0;
					uint32_t cpos = 0;
					uint16_t encl = 0;
					uint16_t vel =  0;

					encl = (uint16_t)((uint16_t)uart3_rxbuffer[8] << 8 |
									  (uint16_t)uart3_rxbuffer[9] << 0 );

					MortorValue[uart2_rxbuffer[0]-1].encl_value = encl;

					tpos = (uint32_t)(
									  ((uint32_t)uart3_rxbuffer[11] << 24) |
									  ((uint32_t)uart3_rxbuffer[12] << 16) |
									  ((uint32_t)uart3_rxbuffer[13] << 8 ) |
									  ((uint32_t)uart3_rxbuffer[14] << 0 )
									  );

					tpos /= gear_ratio[uart2_rxbuffer[0]-1];

					MortorValue[uart2_rxbuffer[0]-1].tpos_value = (float)tpos * 360.00f / 65535.00f;


					cpos = (uint32_t)(
									 ((uint32_t)uart3_rxbuffer[19] << 24) |
									 ((uint32_t)uart3_rxbuffer[20] << 16) |
									 ((uint32_t)uart3_rxbuffer[21] << 8 ) |
									 ((uint32_t)uart3_rxbuffer[22] << 0  )
									 );

					cpos /= gear_ratio[uart2_rxbuffer[0]-1];

					MortorValue[uart2_rxbuffer[0]-1].cpos_value = (float)cpos * 360.00f / 65535.00f;
					++currentIndex;


				}
				else
				{
					Flag[0] = uart2_rxbuffer[0]-1;
					Flag[1] = uart3_rxbuffer[2];
					Flag[2] = uart3_rxbuffer[3];
					HAL_UART_Transmit_DMA(&huart2, Flag, 3);
				}
				break;


			case 0x33:

				if (currentIndex+1 < MOTORS )
				{
					++currentIndex;
					uint32_t tpos = 0;
					tpos = (uint32_t)((uint32_t)uart3_rxbuffer[3] << 24 |
									  (uint32_t)uart3_rxbuffer[4] << 16 |
									  (uint32_t)uart3_rxbuffer[5] << 8  |
									  (uint32_t)uart3_rxbuffer[6] << 0  );

				//MortorValue[currentIndex].tpos_value = (float)tpos * 360.00f / 65536.00f;
				//Emm_V5_Read_Sys_Params(currentIndex, 8);


				}
				break;
			case 0x36:

				if (currentIndex+1 < MOTORS )
				{
					++currentIndex;
					uint32_t cpos = 0;
					cpos = (uint32_t)((uint32_t)uart3_rxbuffer[3] << 24 |
									  (uint32_t)uart3_rxbuffer[4] << 16 |
									  (uint32_t)uart3_rxbuffer[5] << 8  |
									  (uint32_t)uart3_rxbuffer[6] << 0  );

				MortorValue[currentIndex].cpos_value = (float)cpos * 360.00f / 65536.00f;
				Emm_V5_Read_Sys_Params(currentIndex, 10);

				}
				break;
			}
		}

		HAL_UARTEx_ReceiveToIdle_DMA(&huart3, (uint8_t *)uart3_rxbuffer, FLAG_SIZE);
		__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
	}
	
}
