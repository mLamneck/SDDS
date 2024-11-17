#ifndef UPROGFLASH_H
#define UPROGFLASH_H

#define FLASH_USER_START_ADDR 0x0807F000
#define FLASH_USER_END_ADDR   0x0807FFFF

namespace mhal{
	class TflashProg{
	public:
			TflashProg()
			{
					FpageSize = (isDualBank() ? 0x800 : 0x1000);  // Dual-Bank: 2 KB (0x800), Single-Bank: 4 KB (0x1000)
			}

			uint8_t addrToPage(uint32_t _addr){
				return (_addr - FLASH_BASE) / FpageSize;
			}

			static uint32_t erasePage(int _page){
				HAL_StatusTypeDef status;
				HAL_FLASH_Unlock();

				/* Clear OPTVERR bit set on virgin samples */
				__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

				FLASH_EraseInitTypeDef eraseInitStruct;
				uint32_t sectorError = 0;
				eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
				eraseInitStruct.Banks = FLASH_BANK_1;
				eraseInitStruct.Page = _page;
				eraseInitStruct.NbPages = 1;

				status = HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError);
				if (status != HAL_OK)
				{
						HAL_FLASH_Lock();
						return 1;
				}
				HAL_FLASH_Lock();
				return sectorError;
			}

			HAL_StatusTypeDef Write(uint32_t start_address, const void* data, uint32_t data_size)
			{
					HAL_StatusTypeDef status;
					uint32_t address = start_address;
					const uint64_t* data_ptr = (const uint64_t*) data;
					uint32_t full_doublewords = data_size / 8;
					uint32_t remaining_bytes = data_size % 8;
					uint64_t temp_data = 0;

					uint32_t pages_to_erase = (data_size + FpageSize - 1) / FpageSize;

					HAL_FLASH_Unlock();

					/* Clear OPTVERR bit set on virgin samples */
					__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

					FLASH_EraseInitTypeDef eraseInitStruct;
					uint32_t sectorError = 0;
					eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
					eraseInitStruct.Banks = FLASH_BANK_1;
					eraseInitStruct.Page = (start_address - FLASH_BASE) / FpageSize;
					eraseInitStruct.NbPages = pages_to_erase;

					status = HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError);
					if (status != HAL_OK)
					{
							HAL_FLASH_Lock();
							return status == HAL_ERROR ? (HAL_StatusTypeDef)10 : status;
					}

					for (uint32_t i = 0; i < full_doublewords; i++)
					{
							status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, *data_ptr);
							if (status != HAL_OK)
							{
									HAL_FLASH_Lock();
									return status == HAL_ERROR ? (HAL_StatusTypeDef)11 : status;
							}
							address += 8;
							data_ptr++;
					}

					if (remaining_bytes > 0)
					{
							temp_data = 0;
							memcpy(&temp_data, data_ptr, remaining_bytes);
							status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, temp_data);
							if (status != HAL_OK)
							{
									HAL_FLASH_Lock();
									return status == HAL_ERROR ? (HAL_StatusTypeDef)12 : status;
							}
					}

					HAL_FLASH_Lock();

					return HAL_OK;
			}

			HAL_StatusTypeDef Read(uint32_t start_address, void* buffer, uint32_t data_size)
			{
					/*
					if (start_address < FLASH_USER_START_ADDR || start_address + data_size > FLASH_USER_END_ADDR)
					{
							return HAL_ERROR;
					}
					*/
					memcpy(buffer, (const void*)start_address, data_size);
					return HAL_OK;
			}

			static bool readByte(uint32_t _srcAddr, uint8_t& _byte){
				_byte = *(reinterpret_cast<uint8_t*>(_srcAddr));
				return true;
			}

			bool isDualBank()
			{
					FLASH_OBProgramInitTypeDef OBInit;
					HAL_FLASHEx_OBGetConfig(&OBInit);
					return ((OBInit.USERConfig) & (OB_BFB2_ENABLE));
			}

			uint32_t FpageSize;

	};
}
/*

HAL_StatusTypeDef Flash_Write(uint32_t start_address, const void* data, uint32_t data_size)
{
    HAL_StatusTypeDef status;
    uint32_t address = start_address;
    const uint64_t* data_ptr = (const uint64_t*) data;
    uint32_t full_doublewords = data_size / 8;
    uint32_t remaining_bytes = data_size % 8;
    uint64_t temp_data = 0;

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t sectorError = 0;
    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInitStruct.Banks = FLASH_BANK_1;
    eraseInitStruct.Page = (start_address - FLASH_BASE) / FLASH_SECTOR_SIZE;
    eraseInitStruct.NbPages = 1;

    status = HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError);
    if (status != HAL_OK)
    {
        HAL_FLASH_Lock();
        return status;
    }

    for (uint32_t i = 0; i < full_doublewords; i++)
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, *data_ptr);
        if (status != HAL_OK)
        {
            HAL_FLASH_Lock();
            return status;
        }
        address += 8;
        data_ptr++;
    }

    if (remaining_bytes > 0)
    {
        temp_data = 0;
        memcpy(&temp_data, data_ptr, remaining_bytes);
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, temp_data);
        if (status != HAL_OK)
        {
            HAL_FLASH_Lock();
            return status;
        }
    }

    HAL_FLASH_Lock();

    return HAL_OK;
}

HAL_StatusTypeDef Flash_Write_FAST(uint32_t start_address, const void* data, uint32_t data_size)
{
    HAL_StatusTypeDef status;
    uint32_t address = start_address;
    const uint64_t* data_ptr = (const uint64_t*) data;
    uint32_t full_rows = data_size / (FLASH_NB_DOUBLE_WORDS_IN_ROW * 8);
    uint32_t remaining_words = (data_size % (FLASH_NB_DOUBLE_WORDS_IN_ROW * 8)) / 8;
    uint32_t remaining_bytes = data_size % 8;
    uint64_t temp_data = 0;

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t sectorError = 0;
    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInitStruct.Banks = FLASH_BANK_1;
    eraseInitStruct.Page = (start_address - FLASH_BASE) / FLASH_SECTOR_SIZE;
    eraseInitStruct.NbPages = 1;

    status = HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError);
    if (status != HAL_OK)
    {
        HAL_FLASH_Lock();
        return status;
    }

    for (uint32_t i = 0; i < full_rows; i++)
    {
        uint32_t program_type = (i == full_rows - 1 && remaining_words == 0 && remaining_bytes == 0)
                                ? FLASH_TYPEPROGRAM_FAST_AND_LAST
                                : FLASH_TYPEPROGRAM_FAST;

        status = HAL_FLASH_Program(program_type, address, (uint64_t)data_ptr);
        if (status != HAL_OK)
        {
            HAL_FLASH_Lock();
            return status;
        }
        address += FLASH_NB_DOUBLE_WORDS_IN_ROW * 8;
        data_ptr += FLASH_NB_DOUBLE_WORDS_IN_ROW;
    }

    if (remaining_words > 0)
    {
        for (uint32_t i = 0; i < remaining_words; i++)
        {
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, *data_ptr);
            if (status != HAL_OK)
            {
                HAL_FLASH_Lock();
                return status;
            }
            address += 8;
            data_ptr++;
        }
    }

    if (remaining_bytes > 0)
    {
        memcpy(&temp_data, data_ptr, remaining_bytes);
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, temp_data);
        if (status != HAL_OK)
        {
            HAL_FLASH_Lock();
            return status;
        }
    }

    HAL_FLASH_Lock();
    return HAL_OK;
}
*/
#endif
