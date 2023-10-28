#include "uPlatform.h"

#ifndef ULINKEDLIST_H
#define ULINKEDLIST_H

class _TlinkedList;
class _TlinkedListIterator;


/************************************************************************************
_TlinkedListElement
*************************************************************************************/

class _TlinkedListElement{
    friend class _TlinkedList;
    friend class _TlinkedListIterator;
    public:
        typedef _TlinkedListElement T;

    private:
        T* Fnext = this;

        void setUnlinked() { Fnext = this; };
    public:
        T* next() { return Fnext; }
        bool linked(){ return (Fnext != this); };
};


/************************************************************************************
_TlinkedList
*************************************************************************************/

class _TlinkedList : public _TlinkedListElement{
    public:
        typedef _TlinkedListElement T;
        _TlinkedList() { Fnext = nullptr; }
    private:
        void setFirst(T* _first){ Fnext = _first; };
    public:
        T* first() { return Fnext; }

        _TlinkedListIterator iterator();

        T* pop(){
            T* result = first();
            if (result){
                Fnext = result->next();
                result->setUnlinked();
            }
            return result;
        }
        
        void push_first(T* _element){
            if (_element->linked()) return;
            _element->Fnext = first();
            setFirst(_element);
        }

        void push_back(T* _element);

        bool remove(T* _element);
};


/************************************************************************************
_TlinkedListIterator - implements hasNext, next, insert
*************************************************************************************/

class _TlinkedListIterator{
    typedef _TlinkedListElement T;
    protected:
        T* Fcurr = nullptr;
        T* Fprev = nullptr;
    public:
        _TlinkedListIterator(T* _list){
            Fcurr = _list;
            Fprev = nullptr;
        }

        inline bool hasNext(){
            bool _hasNext = (Fcurr->next() != nullptr);
            if (!_hasNext){ Fprev = Fcurr; }
            return _hasNext;
        }
        
        // this is not neccessarily the previous element, but could also be the list itself.
        // or the last element in the list.
        T* prev() { return Fprev; };

        T* next() {
            Fprev = Fcurr;
            Fcurr = Fcurr->next();
            return Fcurr;
        }

        void insert(T* _newElement){
            if (_newElement == nullptr){ return; }
            if (_newElement->linked()){ return; }
            _newElement->Fnext = Fprev->next();
            Fprev->Fnext = _newElement;
        }
};


/************************************************************************************
TlinkedListIterator - with type specification
*************************************************************************************/

template<class elementType> class TlinkedListIterator: public _TlinkedListIterator{
    public:
        using _TlinkedListIterator::_TlinkedListIterator;
        elementType* next() { return static_cast<elementType*>(_TlinkedListIterator::next()); }
        
        //this doesn't work because there's not way to detect if prev points to the list itself
        //elementType* prev() { return (Fcurr==Fprev)?nullptr:static_cast<elementType*>(Fprev); }
};


/************************************************************************************
TlinkedList - with type specification
*************************************************************************************/

template<class elementType> class TlinkedList : public _TlinkedList{
    public:
        TlinkedListIterator<elementType> iterator() { return TlinkedListIterator<elementType>(this); }
        elementType* first() { return (elementType*)_TlinkedList::first(); };
        elementType* pop() { return (elementType*)_TlinkedList::pop(); }
        void push_back(elementType* _element){ _TlinkedList::push_back(_element); };
};


/************************************************************************************
TlinkedListElement
*************************************************************************************/

typedef _TlinkedListElement TlinkedListElement;

#endif //ULINKEDLIST_H
