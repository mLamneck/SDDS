#ifndef UTIME_H
#define UTIME_H

#include "uPlatform.h"
#include "uStrings.h"

dtypes::string timeToString(timeval& _time);

/** \brief class used to parse a string into a TdateTime
 *
 */
class TdateTimeParser{
    typedef dtypes::TdateTime TdateTime;
    private:
        TstringRef Fp;
        dtypes::uint16 Fval;
        tm Ftm;
        dtypes::uint16 Fmillisecs;

        bool adjustDateRec(){
            Ftm.tm_mon--;
            if (Ftm.tm_year < 99) Ftm.tm_year += 2000;
            Ftm.tm_year -= 1900;
            Fp.next();
            return true;
        }

        /** \brief parse a date of the form //08/31/1983 into Ftm structure
         *
         * \param _firstDigit first digit parsed by parse method
         * \return true if successfully parsed
         *
         */
        bool parseDate1(dtypes::uint16 _firstDigit){
            Ftm.tm_mon = _firstDigit;
            if (!Fp.parseValue(Ftm.tm_mday)) return false;
            if (Fp.next() != '/') return false;
            if (!Fp.parseValue(Ftm.tm_year)) return false;
            return adjustDateRec();
        }

        /** \brief parse a date of the form 1983-08-31 into Ftm structure
         *
         * \param _firstDigit first digit parsed by parse method
         * \return true if successfully parsed
         *
         */
        bool parseDate2(dtypes::uint16 _firstDigit){
            Ftm.tm_year = _firstDigit;
            if (!Fp.parseValue(Ftm.tm_mon)) return false;
            if (Fp.next() != '-') return false;
            if (!Fp.parseValue(Ftm.tm_mday)) return false;
            return adjustDateRec();
        }

        /** \brief parse a date of the form 31.08.1983 into Ftm structure
         *
         * \param _firstDigit first digit parsed by parse method
         * \return true if successfully parsed
         *
         */
        bool parseDate3(dtypes::uint16 _firstDigit){
            Ftm.tm_mday = _firstDigit;
            if (!Fp.parseValue(Ftm.tm_mon)) return false;
            if (Fp.next() != '.') return false;
            if (!Fp.parseValue(Ftm.tm_year)) return false;
            return adjustDateRec();
        }

        /** \brief parse a time of the form hh:mm:ss[.zzz] into Ftm/milliseconds structure
         *
         * \param _firstDigit first digit parsed by parse method
         * \param _sep separator that seperates the first digit.
         * \return true if successfully parsed
         *
         */
        bool parseTime(dtypes::uint16 _firstDigit, char _sep){
            Fmillisecs = 0;
            Ftm.tm_hour = _firstDigit+1;
            switch(_sep){
                case '\0': return true;
                case ':': break;
                default : return false;
            }
            if (!Fp.parseValue(Ftm.tm_min)) return false;
            switch(Fp.next()){
                case '\0': return true;
                case ':': break;
                default : return false;
            }
            if (!Fp.parseValue(Ftm.tm_sec)) return false;
            switch(Fp.next()){
                case '\0': return true;
                case '.': {
                    if (!Fp.parseValue(Fmillisecs)) return false;
                    if (Fmillisecs > 999) return false;
                    break;
                };
                default : return false;
            }

            //last adjustments in time
            return true;
        }

    public:
        TdateTime Fresult;

        TdateTimeParser(const char* _input) : Fp(_input){}
        TdateTimeParser() : Fp(""){}

         /** \brief parse a dateTime string into a TdateTime variable. Possible Formats:\n
         * 2022-08-1983[ 12:32:14[.123]]\n
         * 31.08.1983[ 12:32:14[.123]]\n
         * 08/31/1983[ 12:32:14[.123]]\n
         *
         * \return true if successfully parsed
         *
         */
        bool parse(){
            Ftm = {};
            dtypes::uint16 val;
            if (!Fp.parseValue(val)) return false;
            char sep = Fp.next();
            switch(sep){
                case '/' : {
                    if (!parseDate1(val)) return false;
                    break;
                }
                case '-' : {
                    if (!parseDate2(val)) return false;
                    break;
                }
                case '.' : {
                    if (!parseDate3(val)) return false;
                    break;
                }
                case ':' : {
                    if (!parseTime(val,sep)) return false;
                    break;
                }
                default : return false;
            }
            if (Fp.hasNext()){
                if (!Fp.parseValue(val)) return false;
                if (!parseTime(val,Fp.next())) return false;
            }

            //date parsed???
            if (Ftm.tm_year > 0){
                dtypes::uint16 year = Ftm.tm_year;
                dtypes::uint8 mon = Ftm.tm_mon;
                dtypes::uint8 day = Ftm.tm_mday;
                Fresult.tv_sec = mktime(&Ftm);
                if (Fresult.tv_sec == -1) return false;
                //mktime doesn't necessarily fail with sec=-1 for wrong inputs, but the following seems to detect these failures.
                if (Ftm.tm_year != year || Ftm.tm_mon != mon || Ftm.tm_mday != day) return false;
            }

            //if no date parsed, calc manually as mktime seems to be not reliable
            else Fresult.tv_sec = 60 * ((Ftm.tm_hour-1)*60 + Ftm.tm_min) + Ftm.tm_sec;

            Fresult.tv_usec = Fmillisecs*1000;
            return true;
        }

        bool parse(const char* _input){
            Fp = TstringRef(_input);
            return parse();
        }
};

#endif // UTIME_H


