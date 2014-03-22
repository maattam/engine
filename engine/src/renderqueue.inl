//
//  Author   : Matti Määttä
//  Summary  : 
//

template<typename Comparator>
 void RenderQueue::sort(const Comparator& lessThan)
 {
     for(auto it = stacks_.begin(); it != stacks_.end(); ++it)
     {
         qStableSort(it->begin(), it->end(), lessThan);
     }
 }