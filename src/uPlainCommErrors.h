#ifndef UPLAINCOMMERRORS_H
#define UPLAINCOMMERRORS_H

namespace plainComm{
    sdds_enumClass(Terror
        ,___
        ,portParseErr   //1
        ,pathNotFound   //2
        ,pathNoStruct   //3
        ,pathNullPtr    //4
        ,invPort        //5
        ,invFunc        //6
    )
}

#endif