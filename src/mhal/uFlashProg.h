#ifndef MHAL_UFLASHPROG_H_
#define MHAL_UFLASHPROG_H_

#include "uPlatform.h"

#if defined(STM32_CUBE)
	#include "STM32/uFlashProg.h"
#elif MARKI_DEBUG_PLATFORM

	namespace mhal{

		class TflashProg{
		public:
			constexpr static int FLASH_START_ADDR = 0;
			typedef int HAL_StatusTypeDef;
			dtypes::uint8 Fflash[0x7FFFF];

			TflashProg()
			{
				FpageSize = (isDualBank() ? 0x800 : 0x1000);  // Dual-Bank: 2 KB (0x800), Single-Bank: 4 KB (0x1000)
			}		

			uint32_t lastError(){ return 0; }

			constexpr uint32_t getFlashSize(){
				return 0x7FFFF;
			}
		
			constexpr uint32_t getFlashStartAddr(){
				return 0;
			}

			uint8_t getPageFromAbsAddr(uint32_t _addr){
				return (_addr - FLASH_START_ADDR) / FpageSize;
			}
		
			uint8_t getPageFromRelAddr(uint32_t _addr){
				return getPageFromAbsAddr(_addr+FLASH_START_ADDR);
			}
		
			uint32_t getPageSize(uint32_t _pageNum){
				return FpageSize;
			}
		
			uint32_t getAbsAddrFromPage(uint32_t _pageNum) {
				return (_pageNum * FpageSize) + FLASH_START_ADDR;
			}
		
			uint32_t getRelAddrFromPage(uint32_t _addr) {
				return (_addr * FpageSize);
			}
		
			static uint32_t erasePage(int _page){
				return 0;
			}
		
			bool Write(uint32_t start_address, const void* data, uint32_t data_size){
				auto src = static_cast<const uint8_t*>(data);
				while (data_size-- > 0){
					Fflash[start_address++] = *(src++);
				}
				return true;
			}
		
			bool Read(uint32_t start_address, void* buffer, uint32_t data_size){
				auto dst = static_cast<uint8_t*>(buffer);
				while (data_size-- > 0){
					*(dst++) = Fflash[start_address++];
				}
				return true;
			}
		
			bool readByte(uint32_t _srcAddr, uint8_t& _byte){
				_byte = Fflash[_srcAddr];
				return true;
			}
		
			bool isDualBank(){
				return false;
			}
		
			uint32_t FpageSize;
		
		};

	}
	
#else
	#error "uProgFlash not implemented yet for this platform"
#endif


#endif /* MHAL_UFLASHPROG_H_ */
