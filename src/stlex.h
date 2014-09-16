#ifndef _stlex_h_
#define _stlex_h_

template <class Container, class ObjType> bool EraseFromSTLContainer(Container& container, ObjType object)
{
	bool bRemoved = false;
	Container::iterator iter = container.begin();
	for ( ; iter != container.end(); ++iter)
	{
		if (*iter == object)
		{
			container.erase(iter);
			bRemoved = true;
			break;
		}
	}
	return bRemoved;
}

#endif //_stlex_h_