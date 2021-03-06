#ifndef DF_LOGSTARTUP_H
#define DF_LOGSTARTUP_H

#include <AP_HAL/AP_HAL.h>
#include <AP_Mission/AP_Mission.h>

class DataFlash_Class;

class DFMessageWriter {
public:
    DFMessageWriter(DataFlash_Class &DataFlash) :
        _DataFlash(DataFlash) { }

    virtual void reset() = 0;
    virtual void process() = 0;
    virtual bool finished() { return _finished; };

protected:
    bool _finished;
    DataFlash_Class &_DataFlash;
};


class DFMessageWriter_WriteSysInfo : public DFMessageWriter {
public:
    DFMessageWriter_WriteSysInfo(DataFlash_Class &DataFlash,
                                 const prog_char_t *firmware_string) :
        DFMessageWriter(DataFlash),
        _firmware_string(firmware_string)
        { }

    void reset();
    void process();

private:
    enum write_sysinfo_blockwriter_stage {
        ws_blockwriter_stage_init,
        ws_blockwriter_stage_firmware_string,
        ws_blockwriter_stage_git_versions,
        ws_blockwriter_stage_system_id
    };
    write_sysinfo_blockwriter_stage stage;

    const prog_char_t *_firmware_string;
};

class DFMessageWriter_WriteEntireMission : public DFMessageWriter {
public:
    DFMessageWriter_WriteEntireMission(DataFlash_Class &DataFlash) :
        DFMessageWriter(DataFlash)
        {}
    void reset();
    void process();

    void set_mission(const AP_Mission *mission);

private:
    enum entire_mission_blockwriter_stage {
        em_blockwriter_stage_init,
        em_blockwriter_stage_write_new_mission_message,
        em_blockwriter_stage_write_mission_items,
        em_blockwriter_stage_done
    };

    const AP_Mission *_mission = nullptr;
    uint16_t _mission_number_to_send = 0;
    entire_mission_blockwriter_stage stage = em_blockwriter_stage_init;
};

class DFMessageWriter_DFLogStart : public DFMessageWriter {
public:
    DFMessageWriter_DFLogStart(DataFlash_Class &DataFlash,
                               const prog_char_t *firmware_string) :
        DFMessageWriter{DataFlash},
        _writesysinfo(DataFlash, firmware_string),
        _writeentiremission(DataFlash)
        {
        }

    void reset();
    void process();

    void set_mission(const AP_Mission *mission);

private:

    enum log_start_blockwriter_stage {
        ls_blockwriter_stage_init,
        ls_blockwriter_stage_formats,
        ls_blockwriter_stage_parms,
        ls_blockwriter_stage_sysinfo,
        ls_blockwriter_stage_write_entire_mission,
        ls_blockwriter_stage_vehicle_messages,
        ls_blockwriter_stage_done,
    };

    log_start_blockwriter_stage stage;

    uint16_t next_format_to_send;
    AP_Param::ParamToken token;
    AP_Param *ap;
    enum ap_var_type type;
    uint16_t num_format_types;
    const struct LogStructure *_structures;


    DFMessageWriter_WriteSysInfo _writesysinfo;
    DFMessageWriter_WriteEntireMission _writeentiremission;
};

#endif

