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
    while (it.hasCurrent()){
        auto el = it.current();
        if (el == _element){
            auto prev = it.prev();
            prev->Fnext = el->next();
            el->setUnlinked();
            return true;
        }
		it.jumpToNext();
    }
    return false;
}

_TlinkedList::T* _TlinkedList::pop_last(){
	auto it = iterator();
	if (!it.hasCurrent()) return nullptr;
	while (it.hasCurrent() && (it.current()->next() != nullptr))
		it.jumpToNext();
	it.removeCurr();
	return it.current();
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
    for (auto it = iterator(); it.hasCurrent(); ){
        if (it.current() == _element){ return idx; }
        idx++;
		it.jumpToNext();
    }
    return -1;
}
