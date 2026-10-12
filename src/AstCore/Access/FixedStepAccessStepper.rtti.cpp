#include "FixedStepAccessStepper.hpp"

AST_NAMESPACE_BEGIN

Class FixedStepAccessStepper::staticType;

static bool FixedStepAccessStepper_ClassInited = (FixedStepAccessStepper::ClassInit(&FixedStepAccessStepper::staticType), true);

void FixedStepAccessStepper::ClassInit(Class* cls)
{
    cls->setName(NC_("Class", "FixedStepAccessStepper"));
    cls->addToRegistry();
    cls->setParent<AccessStepper>();
    cls->setConstructor<FixedStepAccessStepper>();
}

AST_NAMESPACE_END
