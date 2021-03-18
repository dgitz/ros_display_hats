/*! \file DisplayHatProcess.h
 */
#ifndef DisplayHatProcess_H
#define DisplayHatProcess_H
#include <eros/BaseNodeProcess.h>
/*! \class DisplayHatProcess DisplayHatProcess.h "DisplayHatProcess.h"
 *  \brief */
class DisplayHatProcess : public BaseNodeProcess
{
   public:
    DisplayHatProcess();
    ~DisplayHatProcess();
    Diagnostic::DiagnosticDefinition finish_initialization();
    void reset();
    Diagnostic::DiagnosticDefinition update(double t_dt, double t_ros_time);
    std::vector<Diagnostic::DiagnosticDefinition> new_commandmsg(eros::command msg);
    std::vector<Diagnostic::DiagnosticDefinition> check_programvariables();
    void cleanup() {
        base_cleanup();
        return;
    }

   private:
};
#endif  // DisplayHatProcess_H
