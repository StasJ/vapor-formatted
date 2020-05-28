#pragma once

#include "PRefinementSelector.h"
#include "PWidgetHLI.h"

template <class P>
class PRefinementSelectorHLI : public PRefinementSelector, public PWidgetHLIBase<P, long> {
  public:
    PRefinementSelectorHLI(typename PWidgetHLIBase<P, long>::GetterType getter,
                           typename PWidgetHLIBase<P, long>::SetterType setter)
        : PRefinementSelector(), PWidgetHLIBase<P, long>((PWidget *)this, getter, setter) {}
};
