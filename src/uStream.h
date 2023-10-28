#ifndef USTREAM_H
#define USTREAM_H

class Tstream{
    public:
        virtual void write(const char* _str) = 0;
        virtual void write(char _char) = 0;
        virtual void write(int _int) = 0;
        virtual void write(const dtypes::string &_str){ write(_str.c_str()); }
        virtual void flush(){};
};

class TstringStream : public Tstream{
  private:
    dtypes::string Fbuffer;
  protected:
  public:
    void write(const char* _str) override{
      Fbuffer += _str;
    }
    void write(const char _char) override{
      Fbuffer += _char;
    }
    void write(int _int) override{
      Fbuffer += strConv::to_string(_int);
    }
    void clear(){
        Fbuffer = "";
    }
    const char* data() { return Fbuffer.c_str(); }
};

#endif //USTREAM_H
