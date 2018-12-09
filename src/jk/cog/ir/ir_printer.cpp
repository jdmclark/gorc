#include "ir_printer.hpp"
#include "jk/cog/vm/opcode.hpp"
#include "log/log.hpp"

gorc::cog::ir_printer::ir_printer(file &program_text, message_table &exports)
    : program_text(program_text)
    , program_stream(program_text)
    , exports(exports)
{
    return;
}

void gorc::cog::ir_printer::finalize()
{
    if(!ends_with_ret) {
        // End program text with ret
        ret();
    }

    // Loop over backpatch map, replacing temporary indices
    for(auto const &backpatch : backpatch_map) {
        int label_id;
        size_t call_site;
        std::tie(label_id, call_site) = backpatch;

        size_t target = label_offsets[label_id];

        program_text.set_position(call_site);
        binary_serialize(program_stream, target);
    }

    // Add exported label offsets to export table
    for(auto const &exported_label_id : exported_label_ids) {
        message_type message;
        label_id lbid;
        std::tie(message, lbid) = exported_label_id;

        size_t target = label_offsets[static_cast<int>(lbid)];

        exports.set_offset(message, target);
    }

    return;
}

gorc::cog::label_id gorc::cog::ir_printer::generate_label()
{
    return label_id(next_label_id++);
}

gorc::cog::label_id gorc::cog::ir_printer::get_named_label(std::string const &name)
{
    auto it = named_label_ids.find(name);
    if(it == named_label_ids.end()) {
        auto next_id = generate_label();
        named_label_ids.emplace(name, next_id);
        return next_id;
    }
    else {
        return it->second;
    }
}

gorc::cog::label_id gorc::cog::ir_printer::get_export_label(message_type mt,
                                                            std::string const &name)
{
    auto next_id = get_named_label(name);
    exported_label_ids[mt] = next_id;
    return next_id;
}

void gorc::cog::ir_printer::label(label_id lid)
{
    // Current position is the label offset
    bool succeeded = label_offsets.emplace(static_cast<int>(lid), program_text.position()).second;
    if(!succeeded) {
        LOG_FATAL(format("label id %d used for multiple branch targets") % static_cast<int>(lid));
    }
}

void gorc::cog::ir_printer::push(value v)
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::push);
    binary_serialize(program_stream, v);
}

void gorc::cog::ir_printer::dup()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::dup);
}

void gorc::cog::ir_printer::load(size_t addr)
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::load);
    binary_serialize(program_stream, addr);
}

void gorc::cog::ir_printer::loadi(size_t addr)
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::loadi);
    binary_serialize(program_stream, addr);
}

void gorc::cog::ir_printer::loadg(size_t addr)
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::loadg);
    binary_serialize(program_stream, addr);
}

void gorc::cog::ir_printer::loadgi(size_t addr)
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::loadgi);
    binary_serialize(program_stream, addr);
}

void gorc::cog::ir_printer::stor(size_t addr)
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::stor);
    binary_serialize(program_stream, addr);
}

void gorc::cog::ir_printer::stori(size_t addr)
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::stori);
    binary_serialize(program_stream, addr);
}

void gorc::cog::ir_printer::storg(size_t addr)
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::storg);
    binary_serialize(program_stream, addr);
}

void gorc::cog::ir_printer::storgi(size_t addr)
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::storgi);
    binary_serialize(program_stream, addr);
}

void gorc::cog::ir_printer::write_branch_instruction(opcode op, label_id lid)
{
    binary_serialize(program_stream, op);

    // Store current location in backpatch map
    backpatch_map.emplace(static_cast<int>(lid), program_text.position());

    // Write placeholder
    binary_serialize(program_stream, size_t(0));
}

void gorc::cog::ir_printer::jmp(label_id lid)
{
    ends_with_ret = false;
    write_branch_instruction(opcode::jmp, lid);
}

void gorc::cog::ir_printer::jal(label_id lid)
{
    ends_with_ret = false;
    write_branch_instruction(opcode::jal, lid);
}

void gorc::cog::ir_printer::bt(label_id lid)
{
    ends_with_ret = false;
    write_branch_instruction(opcode::bt, lid);
}

void gorc::cog::ir_printer::bf(label_id lid)
{
    ends_with_ret = false;
    write_branch_instruction(opcode::bf, lid);
}

void gorc::cog::ir_printer::ret()
{
    ends_with_ret = true;
    binary_serialize(program_stream, opcode::ret);
}

void gorc::cog::ir_printer::call(verb_id id, diagnostic_context_location const &loc)
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::call);
    binary_serialize(program_stream, static_cast<int>(id));
    binary_serialize(program_stream, loc.first_line);
    binary_serialize(program_stream, loc.first_col);
    binary_serialize(program_stream, loc.last_line);
    binary_serialize(program_stream, loc.last_col);
}

void gorc::cog::ir_printer::callv(verb_id id, diagnostic_context_location const &loc)
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::callv);
    binary_serialize(program_stream, static_cast<int>(id));
    binary_serialize(program_stream, loc.first_line);
    binary_serialize(program_stream, loc.first_col);
    binary_serialize(program_stream, loc.last_line);
    binary_serialize(program_stream, loc.last_col);
}

void gorc::cog::ir_printer::neg()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::neg);
}

void gorc::cog::ir_printer::lnot()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::lnot);
}

void gorc::cog::ir_printer::add()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::add);
}

void gorc::cog::ir_printer::sub()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::sub);
}

void gorc::cog::ir_printer::mul()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::mul);
}

void gorc::cog::ir_printer::div()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::div);
}

void gorc::cog::ir_printer::mod()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::mod);
}

void gorc::cog::ir_printer::bor()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::bor);
}

void gorc::cog::ir_printer::band()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::band);
}

void gorc::cog::ir_printer::bxor()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::bxor);
}

void gorc::cog::ir_printer::lor()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::lor);
}

void gorc::cog::ir_printer::land()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::land);
}

void gorc::cog::ir_printer::eq()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::eq);
}

void gorc::cog::ir_printer::ne()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::ne);
}

void gorc::cog::ir_printer::gt()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::gt);
}

void gorc::cog::ir_printer::ge()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::ge);
}

void gorc::cog::ir_printer::lt()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::lt);
}

void gorc::cog::ir_printer::le()
{
    ends_with_ret = false;
    binary_serialize(program_stream, opcode::le);
}
