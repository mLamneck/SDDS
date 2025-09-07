#ifndef USTREAM_H
#define USTREAM_H

typedef dtypes::string TstringStreamBuffer;

class Tstream{
    public:
		virtual void init(){};
        virtual void write(const char* _str) = 0;
        virtual void write(char _char) = 0;
        virtual void write(const dtypes::string &_str){ write(_str.c_str()); }
        virtual void flush(){};
        template<typename T>
        void write(T _int){ write(strConv::to_string(_int)); };
};

class TstringStream : public Tstream{
  private:
    dtypes::string Fbuffer;
    
    //FpBuffer is intended to provide a buffer from the parent
    //I want to use it in the webSocketServer to share a buffer
    //with all connected clients...
    dtypes::string* FpBuffer;
  protected:
  public:
    TstringStream(){ FpBuffer = &Fbuffer; } 
    TstringStream(dtypes::string* _buffer) { FpBuffer = _buffer; }
    
    void write(const char* _str) override{
      Fbuffer += _str;
    }
    void write(const char _char) override{
        Fbuffer += _char;
    }
    void clear(){
        Fbuffer = "";
    }
    const char* data() { 
        return Fbuffer.c_str(); 
    }
};

#endif //USTREAM_H
