//
//  Author   : Matti Määttä
//  Summary  : 
//

template<typename Comparator>
 void RenderQueue::sort(const Comparator& lessThan)
 {
     for(RenderMap::Iterator it = queue_.begin(); it != queue_.end(); ++it)
     {
         qStableSort(it->begin(), it->end(), lessThan);
     }
 }