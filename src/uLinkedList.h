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
        _TlinkedListIterator iterator(int _firstIdx);

        T* pop(){
            T* result = first();
            if (result){
                Fnext = result->next();
                result->setUnlinked();
            }
            return result;
        }

		T* pop_last();

        void push_first(T* _element){
			if (_element == nullptr) return;
			if (_element->linked()) return;
            _element->Fnext = first();
            setFirst(_element);
        }

        void push_back(T* _element);

        bool remove(T* _element);

        int  indexOf(T* _element);
        bool hasElements() { return first() != nullptr; }
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
            Fprev = _list;
            Fcurr = _list->next();
        }
        _TlinkedListIterator(T* _list, int _startIdx) : _TlinkedListIterator(_list){
			if (!hasCurrent()) return;
            while (_startIdx-- > 0 && jumpToNext()) ;
        }

		T* hasCurrent(){ return Fcurr; }
		T* jumpToNext() {
			Fprev = Fcurr;
			Fcurr = Fcurr->Fnext;
			return Fcurr;
		}

        // this is not neccessarily the previous element, but could also be the list itself.
        // or the last element in the list.
        T* prev() { return Fprev; };
		T* current() { return Fcurr; }

        void insert(T* _newElement){
            if (_newElement == nullptr){ return; }
            if (_newElement->linked()){ return; }
            _newElement->Fnext = Fprev->next();
            Fprev->Fnext = _newElement;
        }

        void removeCurr(){
            Fprev->Fnext = Fcurr->Fnext;
            Fcurr->setUnlinked();
        }
};


/************************************************************************************
TlinkedListIterator - with type specification
*************************************************************************************/

template<class elementType> class TlinkedListIterator: public _TlinkedListIterator{
    public:
        using _TlinkedListIterator::_TlinkedListIterator;
        elementType* current() { return static_cast<elementType*>(_TlinkedListIterator::current()); }

        //this doesn't work because there's not way to detect if prev points to the list itself
        //elementType* prev() { return (Fcurr==Fprev)?nullptr:static_cast<elementType*>(Fprev); }
};


/************************************************************************************
TlinkedList - with type specification
*************************************************************************************/

template<class elementType> class TlinkedList : public _TlinkedList{
    public:
        TlinkedListIterator<elementType> iterator() { return TlinkedListIterator<elementType>(this); }
        TlinkedListIterator<elementType> iterator(int _firstIdx) { return TlinkedListIterator<elementType>(this,_firstIdx); }
        elementType* first() { return (elementType*)_TlinkedList::first(); };
        elementType* pop() { return (elementType*)_TlinkedList::pop(); }
        elementType* pop_last() { return (elementType*)_TlinkedList::pop_last(); }
        void push_back(elementType* _element){ _TlinkedList::push_back(_element); };
        void push_first(elementType* _element){ _TlinkedList::push_first(_element); };
};


/************************************************************************************
TlinkedListElement
*************************************************************************************/

typedef _TlinkedListElement TlinkedListElement;

#endif //ULINKEDLIST_H
