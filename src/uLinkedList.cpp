#include "uLinkedList.h"


/*********************************************************************************************/
//_TlinkedList implementation
/*********************************************************************************************/

void _TlinkedList::push_back(T* _element){
    if (_element == nullptr){ return; }
    if (_element->linked()){ return; }
    if (first() == nullptr){
        setFirst(_element);
    }
    else{
        T* p = first();
        while (p->next() != nullptr){
            p = p->next();
        }
        p->Fnext = _element;
    }
    _element->Fnext = nullptr;
}

bool _TlinkedList::remove(_TlinkedList::T* _element){
    if (_element == nullptr){ return false; }
    if (!_element->linked()){ return true; }
    auto it = iterator();
    while (it.hasNext()){
        auto el = it.next();
        if (el == _element){
            auto prev = it.prev();
            prev->Fnext = el->next();
            el->setUnlinked();
            return true;
        }
    }
    return false;
}

_TlinkedListIterator _TlinkedList::iterator(){
    return _TlinkedListIterator(this);
}

_TlinkedListIterator _TlinkedList::iterator(int _firstIdx){
    return _TlinkedListIterator(this,_firstIdx);
}

int _TlinkedList::indexOf(_TlinkedList::T* _element)
{
    int idx = 0;
    for (auto it = iterator(); it.hasNext(); ){
        if (it.next() == _element){ return idx; }
        idx++;
    }
    return -1;
}
