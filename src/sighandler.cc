//
// Created by ryotta205 on 9/23/22.
//
#include <stdio.h>
#include "sighandler.h"

int g_num_sig_handle = 0;
bool g_is_sig_term = false;

void (*p_user_signal_handler)(int) = NULL;


SignalHandler::SignalHandler(Generator &generator, Stat &stat, Config &config, FaultResult &fault_result,
                             FaultTrace &fault_trace) :
        generator_(generator), stat_(stat), config_(config), fault_result_(fault_result), fault_trace_(fault_trace) {

    this->num_sig_handle = g_num_sig_handle;
    g_num_sig_handle++;

    if (this->num_sig_handle > 0) {
        this->print_debug_info("SignalHandler() instance can not be created.\n");
        return;
    }

    // init global var/function
    g_is_sig_term = false;
    p_user_signal_handler = NULL;

    this->is_debug_print = false;

    return;
}

SignalHandler::~SignalHandler(void) {
    this->print_debug_info("SignalHandler() instance destructed\n");

    return;
}


void SignalHandler::print_debug_info(const char *_format, ...) {
    if (!this->is_debug_print) return;

    fprintf(stdout, "SignalHandler::");
    va_list arg;
    va_start(arg, _format);
    vprintf(_format, arg);
    va_end(arg);

    return;
}

void SignalHandler::set_debug_print(void) {
    this->is_debug_print = true;

    this->print_debug_info("set_debug_print() set debug print\n");

    return;
}


void SignalHandler::signal_handler(int _sig_num) {
    if (g_is_sig_term) {
        return;
    }

    g_is_sig_term = true;

    if (p_user_signal_handler != NULL) {
        p_user_signal_handler(_sig_num);
    }

    return;
}

void SignalHandler::print_result(int _sig_num) {

    stat_.PrintStat();

    if(config_.fault_trace_on)
        fault_trace_.ResetFaultTrace();
    fault_result_.ResetFaultResult();
    stat_.ResetStat();

    return;
}

void SignalHandler::set_signal(int _sig_num) {
    if (this->num_sig_handle > 0) {
        this->print_debug_info("SignalHandler() instance can not be created.\n");

        return;
    }

    this->print_debug_info("set_signal() bind signal event [%s]\n", strsignal(_sig_num));

    signal(_sig_num, signal_handler);

    return;
}


void SignalHandler::set_ignore(int _sig_num) {
    if (this->num_sig_handle > 0) {
        this->print_debug_info("SignalHandler() instance can not be created.\n");

        return;
    }

    this->print_debug_info("set_ignore() bind ignore event [%s]\n", strsignal(_sig_num));

    signal(_sig_num, SIG_IGN);

    return;
}

void SignalHandler::set_signal_handler(void (*_func)(int)) {
    if (this->num_sig_handle > 0) {
        this->print_debug_info("SignalHandler() instance can not be created.\n");

        return;
    }

    this->print_debug_info("set_signal_handler() bind user term function\n");

    p_user_signal_handler = _func;

    return;
}

bool SignalHandler::is_term(void) {
    return g_is_sig_term;

}