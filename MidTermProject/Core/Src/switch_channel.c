/*
 * switch_channel.c
 *
 *  Created on: Dec 5, 2024
 *      Author: anton
 */

HAL_StatusTypeDef channel_config(ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t sampling_time) {
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = sampling_time;
    if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) {
        Error_Handler();
        return HAL_ERROR;
    }

    return HAL_OK;
}

/*
uint16_t switch_channel(ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t sampling_time) {

	channel_config(hadc, channel, sampling_time);
    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
    uint16_t value = HAL_ADC_GetValue(hadc);
    HAL_ADC_Stop(hadc);

    return value;
}
*/
