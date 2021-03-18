#include "DisplayHatProcess.h"

DisplayHatProcess::DisplayHatProcess() {
}
DisplayHatProcess::~DisplayHatProcess() {
}
Diagnostic::DiagnosticDefinition DisplayHatProcess::finish_initialization() {
    Diagnostic::DiagnosticDefinition diag;
    return diag;
}
void DisplayHatProcess::reset() {
}
Diagnostic::DiagnosticDefinition DisplayHatProcess::update(double t_dt, double t_ros_time) {
    Diagnostic::DiagnosticDefinition diag = base_update(t_dt, t_ros_time);
    return diag;
}
std::vector<Diagnostic::DiagnosticDefinition> DisplayHatProcess::new_commandmsg(eros::command msg) {
    (void)msg;
    std::vector<Diagnostic::DiagnosticDefinition> diag_list;
    return diag_list;
}
std::vector<Diagnostic::DiagnosticDefinition> DisplayHatProcess::check_programvariables() {
    std::vector<Diagnostic::DiagnosticDefinition> diag_list;
    return diag_list;
}
