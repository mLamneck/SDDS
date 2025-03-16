#ifndef UPROGFLASH_STM32_H
#define UPROGFLASH_STM32_H

#define FLASH_USER_START_ADDR 0x0807F000
#define FLASH_USER_END_ADDR   0x0807FFFF

#include <cstring>	//memcpy

namespace mhal{

class TflashProg{
private:
	HAL_StatusTypeDef FlastError;

public:
	constexpr static dtypes::int32 FLASH_START_ADDR = FLASH_BASE;
	TflashProg()
	{
		FpageSize = (isDualBank() ? 0x800 : 0x1000);  // Dual-Bank: 2 KB (0x800), Single-Bank: 4 KB (0x1000)
	}

	HAL_StatusTypeDef lastError(){ return FlastError; }

	uint32_t getFlashSize(){
		#ifdef FLASHSIZE_BASE
			return (*reinterpret_cast<volatile uint16_t*>(FLASHSIZE_BASE)) * 1024;
		#else
			#error "FLASHSIZE_BASE not defined for this STM32 series"
		#endif
	}

	constexpr uint32_t getFlashStartAddr(){
		return FLASH_BASE;
	}

	uint8_t getPageFromAbsAddr(uint32_t _addr){
		return (_addr - FLASH_BASE) / FpageSize;
	}

	uint8_t getPageFromRelAddr(uint32_t _addr){
		return getPageFromAbsAddr(_addr+FLASH_START_ADDR);
	}

	uint32_t getPageSize(uint32_t _pageNum){
		return FpageSize;
	}

	uint32_t getAbsAddrFromPage(uint32_t _pageNum) {
		return (_pageNum * FpageSize) + FLASH_BASE;
	}

	uint32_t getRelAddrFromPage(uint32_t _pageNum) {
		return (_pageNum * FpageSize);
	}

	bool checkRes(const HAL_StatusTypeDef _res){
		FlastError = _res;
		if (_res != HAL_OK)
		{
			HAL_FLASH_Lock();
			return false;
		}
		return true;
	}

	bool WriteNew(uint32_t start_address, const void* data, uint32_t data_size)
	{
	    uint32_t address = start_address;
	    const uint64_t* data_ptr = (const uint64_t*) data;
	    uint64_t temp_data = 0;

	    uint32_t first_page = (start_address - FLASH_BASE) / FpageSize;
	    uint32_t last_page = ((start_address + data_size - 1) - FLASH_BASE) / FpageSize;
	    uint32_t pages_to_erase = (last_page - first_page) + 1;

	    HAL_FLASH_Unlock();
	    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

	    FLASH_EraseInitTypeDef eraseInitStruct = {0};
	    uint32_t sectorError = 0;
	    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	    eraseInitStruct.Banks = FLASH_BANK_1;
	    eraseInitStruct.Page = first_page;
	    eraseInitStruct.NbPages = pages_to_erase;

	    if (!checkRes(HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError)))
	    	return false;

	    uint32_t full_32doublewords = data_size / (8 * 32);
	    for (uint32_t i = 0; i < full_32doublewords; i++)
	    {
	    	if (!checkRes(HAL_FLASH_Program(i == full_32doublewords - 1 ? FLASH_TYPEPROGRAM_FAST_AND_LAST : FLASH_TYPEPROGRAM_FAST, address, (uint64_t)data_ptr)))
	        	return false;
	        address += 8 * 32;
	        data_ptr += 32;
	    }

	    uint32_t full_doublewords = (data_size - (full_32doublewords * 8 * 32)) / 8;
	    for (uint32_t i = 0; i < full_doublewords; i++)
	    {
	        if (!checkRes(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, *data_ptr)))
	        	return false;
	        address += 8;
	        data_ptr++;
	    }

	    uint32_t remaining_bytes = data_size % 8;
	    if (remaining_bytes > 0)
	    {
	        temp_data = 0;
	        memcpy(&temp_data, data_ptr, remaining_bytes);
	        if (!checkRes(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, temp_data)))
	        	return false;
	    }

	    HAL_FLASH_Lock();
	    return true;
	}

	bool Write(uint32_t start_address, const void* data, uint32_t data_size)
	{
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

		if (!checkRes(HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError)) ||  sectorError != 0xFFFFFFFF)
			return false;

		for (uint32_t i = 0; i < full_doublewords; i++)
		{
			if (!checkRes(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, *data_ptr)))
				return false;
			address += 8;
			data_ptr++;
		}

		if (remaining_bytes > 0)
		{
			temp_data = 0;
			memcpy(&temp_data, data_ptr, remaining_bytes);
			if (!checkRes(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, temp_data)))
				return false;
		}

		HAL_FLASH_Lock();
		return true;
	}

	HAL_StatusTypeDef WriteOrig(uint32_t start_address, const void* data, uint32_t data_size)
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
		if (status != HAL_OK || sectorError != 0xFFFFFFFF)
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

#endif //UPROGFLASH_STM32_H
