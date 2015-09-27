#include "program/program.hpp"
#include "cogcheck_compiler.hpp"
#include "io/native_file.hpp"
#include <map>

namespace gorc {

    class cog_check_program : public program {
    private:
        std::vector<std::string> cog_files;
        bool dump_ast = false;
        bool parse_only = false;
        bool disassemble = false;

        cog::verb_table verbs;

    public:
        virtual void create_options(options &opts) override
        {
            opts.insert_bare(make_bare_multi_value_option(std::back_inserter(cog_files)));
            opts.insert(make_switch_option("dump-ast", dump_ast));
            opts.insert(make_switch_option("parse-only", parse_only));
            opts.insert(make_switch_option("disassemble", disassemble));

            opts.emplace_constraint<at_least_one_input>();
            return;
        }

        virtual int main() override
        {
            mock_populate_verb_table();

            cog::constant_table constants;
            cog::default_populate_constant_table(constants);

            cogcheck_compiler compiler(verbs,
                                       constants,
                                       dump_ast,
                                       parse_only,
                                       disassemble);

            bool success = true;
            for(auto const &cog_file : cog_files) {
                diagnostic_context dc(cog_file.c_str());
                try {
                    auto f = make_native_read_only_file(cog_file);
                    compiler.compile(*f);
                }
                catch(logged_runtime_error const &) {
                    success = false;
                }
                catch(std::exception const &e) {
                    success = false;
                    LOG_ERROR(e.what());
                }
            }

            return success ? EXIT_SUCCESS : EXIT_FAILURE;
        }

        void mock_verb(std::string const &str,
                       cog::value_type return_type,
                       std::initializer_list<cog::value_type> argument_types)
        {
            verbs.emplace_verb<cog::mock_verb>(str,
                                               return_type,
                                               std::vector<cog::value_type>(argument_types));
        }

        void mock_populate_verb_table()
        {
            using namespace cog;

            // AI verbs
            mock_verb("aiclearmode", value_type::nothing, { value_type::thing,
                                                            value_type::integer });
            mock_verb("aiflee", value_type::nothing, { value_type::thing,
                                                       value_type::thing });
            mock_verb("aigetmode", value_type::integer, { value_type::thing });
            mock_verb("aijump", value_type::nothing, { value_type::thing,
                                                       value_type::vector,
                                                       value_type::floating });
            mock_verb("aisetclass", value_type::nothing, { value_type::thing,
                                                           value_type::ai });
            mock_verb("aisetfiretarget", value_type::nothing, { value_type::thing,
                                                                value_type::thing });
            mock_verb("aisetlookpos", value_type::nothing, { value_type::thing,
                                                             value_type::vector });
            mock_verb("aisetmode", value_type::nothing, { value_type::thing,
                                                          value_type::integer });
            mock_verb("aisetmovepos", value_type::nothing, { value_type::thing,
                                                             value_type::vector });
            mock_verb("aisetmovespeed", value_type::nothing, { value_type::thing,
                                                               value_type::floating });
            mock_verb("aisetmovething", value_type::nothing, { value_type::thing,
                                                               value_type::thing });
            mock_verb("getactorweapon", value_type::thing_template, { value_type::thing,
                                                                      value_type::integer });
            mock_verb("isaitargetinsight", value_type::boolean, { value_type::thing });
            mock_verb("setactorweapon", value_type::nothing, { value_type::thing,
                                                               value_type::integer,
                                                               value_type::thing_template });

            // Animation
            mock_verb("getmaterialcel", value_type::integer, { value_type::material });
            mock_verb("materialanim", value_type::thing, { value_type::material,
                                                           value_type::floating,
                                                           value_type::integer });
            mock_verb("setmaterialcel", value_type::nothing, { value_type::material,
                                                               value_type::integer });
            mock_verb("getsurfaceanim", value_type::thing, { value_type::surface });
            mock_verb("getsurfacecel", value_type::integer, { value_type::surface });
            verbs.add_synonym("getsurfacecel", "getwallcel");
            mock_verb("setsurfacecel", value_type::nothing, { value_type::surface,
                                                              value_type::integer });
            verbs.add_synonym("setsurfacecel", "setwallcel");
            mock_verb("stopsurfaceanim", value_type::nothing, { value_type::surface });
            mock_verb("surfaceanim", value_type::thing, { value_type::surface,
                                                          value_type::floating,
                                                          value_type::integer });
            mock_verb("slideceilingsky", value_type::thing, { value_type::floating,
                                                              value_type::floating });
            mock_verb("slidehorizonsky", value_type::thing, { value_type::floating,
                                                              value_type::floating });
            mock_verb("slidesurface", value_type::thing, { value_type::surface,
                                                           value_type::vector,
                                                           value_type::floating });
            verbs.add_synonym("slidesurface", "slidewall");
            mock_verb("stopanim", value_type::nothing, { value_type::thing });
            mock_verb("surfacelightanim", value_type::thing, { value_type::surface,
                                                               value_type::floating,
                                                               value_type::floating,
                                                               value_type::floating });

            // Camera
            mock_verb("cyclecamera", value_type::nothing, { });
            mock_verb("getcamerastateflags", value_type::integer, { });
            mock_verb("getcurrentcamera", value_type::integer, { });
            mock_verb("getprimaryfocus", value_type::thing, { value_type::integer });
            mock_verb("setcamerafocus", value_type::nothing, { value_type::integer,
                                                               value_type::thing });
            mock_verb("setcamerastateflags", value_type::nothing, { value_type::integer });
            mock_verb("setcurrentcamera", value_type::nothing, { value_type::integer });
            mock_verb("setpovshake", value_type::nothing, { value_type::vector,
                                                            value_type::vector,
                                                            value_type::floating,
                                                            value_type::floating });

            // Color
            mock_verb("adddynamicadd", value_type::nothing, { value_type::thing,
                                                              value_type::integer,
                                                              value_type::integer,
                                                              value_type::integer });
            mock_verb("adddynamictint", value_type::nothing, { value_type::thing,
                                                               value_type::integer,
                                                               value_type::integer,
                                                               value_type::integer });
            mock_verb("freecoloreffect", value_type::nothing, { value_type::thing });
            mock_verb("newcoloreffect", value_type::thing, { value_type::integer,
                                                             value_type::integer,
                                                             value_type::integer,
                                                             value_type::floating,
                                                             value_type::floating,
                                                             value_type::floating,
                                                             value_type::integer,
                                                             value_type::integer,
                                                             value_type::integer,
                                                             value_type::floating });

            // Creature
            mock_verb("getheadlightintensity", value_type::floating, { value_type::thing });
            mock_verb("getthinghealth", value_type::floating, { value_type::thing });
            verbs.add_synonym("getthinghealth", "gethealth");
            mock_verb("haslos", value_type::boolean, { value_type::thing,
                                                       value_type::thing });
            mock_verb("healthing", value_type::nothing, { value_type::thing,
                                                          value_type::floating });
            mock_verb("isthingcrouching", value_type::boolean, { value_type::thing });
            verbs.add_synonym("isthingcrouching", "iscrouching");
            mock_verb("jkclearflags", value_type::nothing, { value_type::thing,
                                                             value_type::integer });
            mock_verb("jkgetflags", value_type::integer, { value_type::thing });
            mock_verb("jksetflags", value_type::nothing, { value_type::thing,
                                                           value_type::integer });
            mock_verb("jksetinvis", value_type::nothing, { value_type::thing,
                                                           value_type::integer });
            mock_verb("jksetinvulnerable", value_type::nothing, { value_type::thing,
                                                                  value_type::integer });
            mock_verb("setactorextraspeed", value_type::nothing, { value_type::thing,
                                                                   value_type::floating });
            mock_verb("setheadlightintensity", value_type::nothing, { value_type::thing,
                                                                      value_type::floating });
            mock_verb("setthinghealth", value_type::nothing, { value_type::thing,
                                                               value_type::floating });
            verbs.add_synonym("setthinghealth", "sethealth");

            // Frame
            mock_verb("aisetlookframe", value_type::nothing, { value_type::thing,
                                                               value_type::integer });
            mock_verb("aisetmoveframe", value_type::nothing, { value_type::thing,
                                                               value_type::integer });
            mock_verb("getcurframe", value_type::integer, { value_type::thing });
            mock_verb("getgoalframe", value_type::integer, { value_type::thing });
            mock_verb("jumptoframe", value_type::nothing, { value_type::thing,
                                                            value_type::integer,
                                                            value_type::sector });
            mock_verb("movetoframe", value_type::nothing, { value_type::thing,
                                                            value_type::integer,
                                                            value_type::floating });
            mock_verb("pathmovepause", value_type::nothing, { value_type::thing });
            mock_verb("pathmoveresume", value_type::nothing, { value_type::thing });
            mock_verb("skiptoframe", value_type::nothing, { value_type::thing,
                                                            value_type::integer,
                                                            value_type::floating });
            mock_verb("waitforstop", value_type::nothing, { value_type::thing });
            mock_verb("rotate", value_type::nothing, { value_type::thing,
                                                       value_type::vector });
            mock_verb("rotatepivot", value_type::nothing, { value_type::thing,
                                                            value_type::integer,
                                                            value_type::floating });

            // Inventory
            mock_verb("changeinv", value_type::nothing, { value_type::thing,
                                                          value_type::integer,
                                                          value_type::floating });
            mock_verb("getinv", value_type::floating, { value_type::thing,
                                                        value_type::integer });
            mock_verb("getinvcog", value_type::cog, { value_type::thing,
                                                      value_type::integer });
            mock_verb("getinvmax", value_type::floating, { value_type::thing,
                                                           value_type::integer });
            mock_verb("getinvmin", value_type::floating, { value_type::thing,
                                                           value_type::integer });
            mock_verb("isinvactivated", value_type::boolean, { value_type::thing,
                                                               value_type::integer });
            mock_verb("isinvavailable", value_type::boolean, { value_type::thing,
                                                               value_type::integer });
            mock_verb("setbinwait", value_type::nothing, { value_type::thing,
                                                           value_type::integer,
                                                           value_type::floating });
            mock_verb("setinv", value_type::nothing, { value_type::thing,
                                                       value_type::integer,
                                                       value_type::floating });
            mock_verb("setinvactivated", value_type::nothing, { value_type::thing,
                                                                value_type::integer,
                                                                value_type::boolean });
            mock_verb("setinvavailable", value_type::nothing, { value_type::thing,
                                                                value_type::integer,
                                                                value_type::boolean });
            mock_verb("setinvflags", value_type::nothing, { value_type::thing,
                                                            value_type::integer,
                                                            value_type::integer });

            // Keyframe
            mock_verb("getkeylen", value_type::floating, { value_type::keyframe });
            mock_verb("getmajormode", value_type::integer, { value_type::thing });
            mock_verb("jkplaypovkey", value_type::thing, { value_type::thing,
                                                           value_type::keyframe,
                                                           value_type::integer,
                                                           value_type::integer });
            mock_verb("jkstoppovkey", value_type::nothing, { value_type::thing,
                                                             value_type::thing,
                                                             value_type::floating });
            mock_verb("playkey", value_type::thing, { value_type::thing,
                                                      value_type::keyframe,
                                                      value_type::integer,
                                                      value_type::integer });
            mock_verb("playmode", value_type::thing, { value_type::thing,
                                                       value_type::integer });
            mock_verb("stopkey", value_type::nothing, { value_type::thing,
                                                        value_type::thing,
                                                        value_type::floating });

            // Level
            mock_verb("autosavegame", value_type::nothing, { });
            mock_verb("getceilingskyoffset", value_type::vector, { });
            mock_verb("getflexgametime", value_type::floating, { });
            mock_verb("getgametime", value_type::integer, { });
            mock_verb("getgravity", value_type::floating, { });
            mock_verb("gethorizonskyoffset", value_type::vector, { });
            mock_verb("getleveltime", value_type::floating, { });
            mock_verb("getsectorcount", value_type::integer, { });
            mock_verb("getsurfacecount", value_type::integer, { });
            mock_verb("getthingcount", value_type::integer, { });
            mock_verb("getthingtemplatecount", value_type::integer, { value_type::thing_template });
            mock_verb("jkendlevel", value_type::nothing, { value_type::boolean });
            mock_verb("setgravity", value_type::nothing, { value_type::floating });

            // Message
            mock_verb("getparam", value_type::dynamic, { value_type::integer });
            mock_verb("getsenderid", value_type::integer, { });
            mock_verb("getsenderref", value_type::dynamic, { });
            mock_verb("getsendertype", value_type::integer, { });
            mock_verb("getsourceref", value_type::dynamic, { });
            mock_verb("getsourcetype", value_type::integer, { });
            mock_verb("killtimerex", value_type::nothing, { value_type::thing });
            mock_verb("returnex", value_type::nothing, { value_type::dynamic });
            mock_verb("sendmessage", value_type::nothing, { value_type::cog,
                                                            value_type::message });
            mock_verb("sendmessageex", value_type::dynamic, { value_type::cog,
                                                              value_type::message,
                                                              value_type::dynamic,
                                                              value_type::dynamic,
                                                              value_type::dynamic,
                                                              value_type::dynamic });
            mock_verb("sendtrigger", value_type::nothing, { value_type::thing,
                                                            value_type::dynamic,
                                                            value_type::dynamic,
                                                            value_type::dynamic,
                                                            value_type::dynamic,
                                                            value_type::dynamic });
            mock_verb("setparam", value_type::nothing, { value_type::integer,
                                                         value_type::dynamic });
            mock_verb("setpulse", value_type::nothing, { value_type::floating });
            mock_verb("settimer", value_type::nothing, { value_type::floating });
            mock_verb("settimerex", value_type::nothing, { value_type::floating,
                                                           value_type::dynamic,
                                                           value_type::dynamic,
                                                           value_type::dynamic });
            mock_verb("sleep", value_type::nothing, { value_type::floating });
            mock_verb("setthingpulse", value_type::nothing, { value_type::thing,
                                                              value_type::floating });
            mock_verb("setthingtimer", value_type::nothing, { value_type::thing,
                                                              value_type::floating });

            // Misc.
            mock_verb("amputatejoint", value_type::nothing, { value_type::thing,
                                                              value_type::integer });
            mock_verb("disableirmode", value_type::nothing, { });
            mock_verb("enableirmode", value_type::nothing, { value_type::floating,
                                                             value_type::floating });
            mock_verb("getsithmode", value_type::integer, { });
            mock_verb("jkdisablesaber", value_type::nothing, { value_type::thing });
            mock_verb("jkenablesaber", value_type::nothing, { value_type::thing,
                                                              value_type::floating,
                                                              value_type::floating,
                                                              value_type::floating });
            mock_verb("jkendtarget", value_type::nothing, { });
            mock_verb("jksetpersuasioninfo", value_type::nothing, { value_type::thing,
                                                                    value_type::floating,
                                                                    value_type::floating });
            mock_verb("jksetsaberinfo", value_type::nothing, { value_type::thing,
                                                               value_type::material,
                                                               value_type::material,
                                                               value_type::floating,
                                                               value_type::floating,
                                                               value_type::floating,
                                                               value_type::thing_template,
                                                               value_type::thing_template,
                                                               value_type::thing_template });
            mock_verb("jksettarget", value_type::nothing, { value_type::thing });
            mock_verb("jksettargetcolors", value_type::nothing, { value_type::integer,
                                                                  value_type::integer,
                                                                  value_type::integer });
            mock_verb("parsearg", value_type::nothing, { value_type::thing,
                                                         value_type::string });
            mock_verb("skilltarget", value_type::dynamic, { value_type::thing,
                                                            value_type::dynamic,
                                                            value_type::dynamic,
                                                            value_type::dynamic });
            mock_verb("takeitem", value_type::nothing, { value_type::thing,
                                                         value_type::thing });
            mock_verb("thingviewdot", value_type::floating, { value_type::thing,
                                                              value_type::thing });

            // Multiplayer
            mock_verb("clearmultimodeflags", value_type::nothing, { value_type::integer });
            mock_verb("createbackpack", value_type::thing, { value_type::thing });
            mock_verb("getabsolutemaxplayers", value_type::integer, { });
            mock_verb("getmaxplayers", value_type::integer, { });
            mock_verb("getmultimodeflags", value_type::integer, { });
            mock_verb("getnumplayers", value_type::integer, { });
            mock_verb("getnumplayersinteam", value_type::integer, { value_type::integer });
            mock_verb("getplayerteam", value_type::integer, { value_type::thing });
            mock_verb("getrespawnmask", value_type::integer, { value_type::thing });
            mock_verb("gettimelimit", value_type::floating, { });
            mock_verb("ismulti", value_type::boolean, { });
            mock_verb("isserver", value_type::boolean, { });
            mock_verb("nthbackpackbin", value_type::integer, { value_type::thing,
                                                               value_type::integer });
            mock_verb("nthbackpackvalue", value_type::floating, { value_type::thing,
                                                                  value_type::integer });
            mock_verb("numbackpackitems", value_type::integer, { value_type::thing });
            mock_verb("pickupbackpack", value_type::nothing, { value_type::thing,
                                                               value_type::thing });
            mock_verb("setmultimodeflags", value_type::nothing, { value_type::integer });
            mock_verb("setplayerteam", value_type::nothing, { value_type::thing,
                                                              value_type::integer });
            mock_verb("setrespawnmask", value_type::nothing, { value_type::thing,
                                                               value_type::integer });
            mock_verb("settimelimit", value_type::nothing, { value_type::floating });
            mock_verb("syncscores", value_type::nothing, { });
            mock_verb("syncsector", value_type::nothing, { value_type::sector });
            mock_verb("syncsurface", value_type::nothing, { value_type::surface });
            mock_verb("syncthingattachment", value_type::nothing, { value_type::thing });
            mock_verb("syncthingpos", value_type::nothing, { value_type::thing });
            mock_verb("syncthingstate", value_type::nothing, { value_type::thing });

            // Options
            mock_verb("getautopickup", value_type::integer, { });
            mock_verb("getautoreload", value_type::integer, { });
            mock_verb("getautoswitch", value_type::integer, { });
            mock_verb("getdifficulty", value_type::integer, { });
            mock_verb("jkgetsabercam", value_type::boolean, { });
            mock_verb("setautopickup", value_type::nothing, { value_type::thing,
                                                              value_type::integer });
            mock_verb("setautoreload", value_type::nothing, { value_type::thing,
                                                              value_type::integer });
            mock_verb("setautoswitch", value_type::nothing, { value_type::thing,
                                                              value_type::integer });

            // Particle
            mock_verb("getparticlegrowthspeed", value_type::floating, { value_type::thing });
            mock_verb("getparticlesize", value_type::floating, { value_type::thing });
            mock_verb("getparticletimeoutrate", value_type::floating, { value_type::thing });
            mock_verb("setparticlegrowthspeed", value_type::nothing, { value_type::thing,
                                                                       value_type::floating });
            mock_verb("setparticlesize", value_type::nothing, { value_type::thing,
                                                                value_type::floating });
            mock_verb("setparticletimeoutrate", value_type::nothing, { value_type::thing,
                                                                       value_type::floating });

            // Player
            mock_verb("cleargoalflags", value_type::nothing, { value_type::thing,
                                                               value_type::integer,
                                                               value_type::integer });
            mock_verb("getlocalplayerthing", value_type::thing, { });
            mock_verb("getplayernum", value_type::player, { value_type::thing });
            mock_verb("getplayerthing", value_type::thing, { value_type::player });
            mock_verb("jkclearsuperflags", value_type::nothing, { value_type::integer });
            mock_verb("jkgetchoice", value_type::integer, { });
            mock_verb("jkgetlocalplayer", value_type::thing, { });
            mock_verb("jkgetsuperflags", value_type::integer, { });
            mock_verb("jksetforcespeed", value_type::nothing, { value_type::floating });
            mock_verb("jksetsuperflags", value_type::nothing, { value_type::integer });
            mock_verb("setgoalflags", value_type::nothing, { value_type::thing,
                                                             value_type::integer,
                                                             value_type::integer });

            // Print
            mock_verb("jkprintunistring", value_type::nothing, { value_type::thing,
                                                                 value_type::integer });
            mock_verb("jkstringclear", value_type::nothing, { });
            mock_verb("jkstringconcatasciistring", value_type::nothing, { value_type::string });
            mock_verb("jkstringconcatflex", value_type::nothing, { value_type::floating });
            mock_verb("jkstringconcatformattedflex", value_type::nothing, { value_type::floating,
                                                                            value_type::string });
            mock_verb("jkstringconcatformattedint", value_type::nothing, { value_type::integer,
                                                                           value_type::string });
            mock_verb("jkstringconcatint", value_type::nothing, { value_type::integer });
            mock_verb("jkstringconcatplayername", value_type::nothing, { value_type::thing });
            mock_verb("jkstringconcatspace", value_type::nothing, { });
            mock_verb("jkstringconcatunistring", value_type::nothing, { value_type::integer });
            mock_verb("jkstringconcatvector", value_type::nothing, { value_type::vector });
            mock_verb("jkstringoutput", value_type::nothing, { value_type::thing,
                                                               value_type::thing });
            mock_verb("print", value_type::nothing, { value_type::string });
            mock_verb("printflex", value_type::nothing, { value_type::floating });
            mock_verb("printint", value_type::nothing, { value_type::integer });
            mock_verb("printvector", value_type::nothing, { value_type::vector });

            // Score
            mock_verb("addscoretoteammembers", value_type::nothing, { value_type::integer,
                                                                      value_type::integer });
            mock_verb("getplayerkilled", value_type::integer, { value_type::thing });
            mock_verb("getplayerkills", value_type::integer, { value_type::thing });
            mock_verb("getplayerscore", value_type::integer, { value_type::thing });
            mock_verb("getplayersuicides", value_type::integer, { value_type::thing });
            mock_verb("getscorelimit", value_type::integer, { });
            mock_verb("getteamscore", value_type::integer, { value_type::integer });
            mock_verb("setplayerkilled", value_type::nothing, { value_type::thing,
                                                                value_type::integer });
            mock_verb("setplayerkills", value_type::nothing, { value_type::thing,
                                                               value_type::integer });
            mock_verb("setplayerscore", value_type::nothing, { value_type::thing,
                                                               value_type::integer });
            mock_verb("setplayersuicides", value_type::nothing, { value_type::thing,
                                                                  value_type::integer });
            mock_verb("setscorelimit", value_type::nothing, { value_type::integer });
            mock_verb("setteamscore", value_type::nothing, { value_type::integer,
                                                             value_type::integer });

            // Sector
            mock_verb("clearsectorflags", value_type::nothing, { value_type::sector,
                                                                 value_type::integer });
            mock_verb("firstthinginsector", value_type::thing, { value_type::sector });
            mock_verb("getnumsectorsurfaces", value_type::integer, { value_type::sector });
            mock_verb("getnumsectorvertices", value_type::integer, { value_type::sector });
            mock_verb("getsectorcenter", value_type::vector, { value_type::sector });
            mock_verb("getsectorcolormap", value_type::colormap, { value_type::sector });
            verbs.add_synonym("getsectorcolormap", "getcolormap");
            mock_verb("getsectorflags", value_type::integer, { value_type::sector });
            mock_verb("getsectorlight", value_type::floating, { value_type::sector });
            mock_verb("getsectorplayercount", value_type::integer, { value_type::sector });
            mock_verb("getsectorsurfaceref", value_type::surface, { value_type::sector,
                                                                    value_type::integer });
            mock_verb("getsectorthingcount", value_type::integer, { value_type::sector });
            mock_verb("getsectorthrust", value_type::vector, { value_type::sector });
            mock_verb("getsectortint", value_type::vector, { value_type::sector });
            mock_verb("getsectorvertexpos", value_type::vector, { value_type::sector,
                                                                  value_type::integer });
            mock_verb("nextthinginsector", value_type::thing, { value_type::thing });
            mock_verb("prevthinginsector", value_type::thing, { value_type::thing });
            mock_verb("sectorplayercount", value_type::integer, { value_type::sector });
            mock_verb("sectorthingcount", value_type::integer, { value_type::sector });
            mock_verb("setsectoradjoins", value_type::nothing, { value_type::sector,
                                                                 value_type::boolean });
            verbs.add_synonym("setsectoradjoins", "sectoradjoins");
            mock_verb("setsectorcolormap", value_type::nothing, { value_type::sector,
                                                                  value_type::colormap });
            verbs.add_synonym("setsectorcolormap", "setcolormap");
            mock_verb("setsectorflags", value_type::nothing, { value_type::sector,
                                                               value_type::integer });
            mock_verb("setsectorlight", value_type::nothing, { value_type::sector,
                                                               value_type::floating,
                                                               value_type::floating });
            verbs.add_synonym("setsectorlight", "sectorlight");
            mock_verb("setsectorthrust", value_type::nothing, { value_type::sector,
                                                                value_type::vector,
                                                                value_type::floating });
            verbs.add_synonym("setsectorthrust", "sectorthrust");
            mock_verb("setsectortint", value_type::nothing, { value_type::sector,
                                                              value_type::vector });

            // Sound
            mock_verb("changesoundpitch", value_type::nothing, { value_type::thing,
                                                                 value_type::floating,
                                                                 value_type::floating });
            mock_verb("changesoundvol", value_type::nothing, { value_type::thing,
                                                               value_type::floating,
                                                               value_type::floating });
            mock_verb("getsoundlen", value_type::floating, { value_type::sound });
            mock_verb("playsong", value_type::nothing, { value_type::integer,
                                                         value_type::integer,
                                                         value_type::integer });
            mock_verb("playsoundclass", value_type::thing, { value_type::thing,
                                                             value_type::integer });
            mock_verb("playsoundglobal", value_type::thing, { value_type::sound,
                                                              value_type::floating,
                                                              value_type::floating,
                                                              value_type::integer });
            mock_verb("playsoundlocal", value_type::thing, { value_type::sound,
                                                             value_type::floating,
                                                             value_type::floating,
                                                             value_type::integer });
            mock_verb("playsoundpos", value_type::thing, { value_type::sound,
                                                           value_type::vector,
                                                           value_type::floating,
                                                           value_type::floating,
                                                           value_type::floating,
                                                           value_type::integer });
            mock_verb("playsoundthing", value_type::thing, { value_type::sound,
                                                             value_type::thing,
                                                             value_type::floating,
                                                             value_type::floating,
                                                             value_type::floating,
                                                             value_type::integer });
            mock_verb("sectorsound", value_type::nothing, { value_type::sector,
                                                            value_type::sound,
                                                            value_type::floating });
            mock_verb("setmusicvol", value_type::nothing, { value_type::floating });
            mock_verb("stopsound", value_type::nothing, { value_type::thing,
                                                          value_type::floating });

            // Special flags
            mock_verb("cleardebugmodeflags", value_type::nothing, { value_type::integer });
            mock_verb("clearmapmodeflags", value_type::nothing, { value_type::integer });
            mock_verb("clearsubmodeflags", value_type::nothing, { value_type::integer });
            mock_verb("getdebugmodeflags", value_type::integer, { });
            mock_verb("getmapmodeflags", value_type::integer, { });
            mock_verb("getsubmodeflags", value_type::integer, { });
            mock_verb("setdebugmodeflags", value_type::nothing, { value_type::integer });
            mock_verb("setmapmodeflags", value_type::nothing, { value_type::integer });
            mock_verb("setsubmodeflags", value_type::nothing, { value_type::integer });

            // Surface
            mock_verb("clearadjoinflags", value_type::nothing, { value_type::surface,
                                                                 value_type::integer });
            mock_verb("clearfacetype", value_type::nothing, { value_type::surface,
                                                              value_type::integer });
            mock_verb("clearsurfaceflags", value_type::nothing, { value_type::surface,
                                                                  value_type::integer });
            mock_verb("getadjoinflags", value_type::integer, { value_type::surface });
            mock_verb("getfacegeomode", value_type::integer, { value_type::surface });
            mock_verb("getfacelightmode", value_type::integer, { value_type::surface });
            mock_verb("getfacetexmode", value_type::integer, { value_type::surface });
            mock_verb("getfacetype", value_type::integer, { value_type::surface });
            mock_verb("getnumsurfacevertices", value_type::integer, { value_type::surface });
            mock_verb("getsurfaceadjoin", value_type::surface, { value_type::surface });
            mock_verb("getsurfacecenter", value_type::vector, { value_type::surface });
            verbs.add_synonym("getsurfacecenter", "surfacecenter");
            mock_verb("getsurfaceflags", value_type::integer, { value_type::surface });
            mock_verb("getsurfacelight", value_type::floating, { value_type::surface });
            mock_verb("getsurfacemat", value_type::material, { value_type::surface });
            mock_verb("getsurfacenormal", value_type::vector, { value_type::surface });
            mock_verb("getsurfacesector", value_type::sector, { value_type::surface });
            mock_verb("getsurfacevertexpos", value_type::vector, { value_type::surface,
                                                                   value_type::integer });
            mock_verb("setadjoinflags", value_type::nothing, { value_type::surface,
                                                               value_type::integer });
            mock_verb("setfacegeomode", value_type::nothing, { value_type::surface,
                                                               value_type::integer });
            mock_verb("setfacelightmode", value_type::nothing, { value_type::surface,
                                                                 value_type::integer });
            mock_verb("setfacetexmode", value_type::nothing, { value_type::surface,
                                                               value_type::integer });
            mock_verb("setfacetype", value_type::nothing, { value_type::surface,
                                                            value_type::integer });
            mock_verb("setsurfaceflags", value_type::nothing, { value_type::surface,
                                                                value_type::integer });
            mock_verb("setsurfacelight", value_type::nothing, { value_type::surface,
                                                                value_type::floating,
                                                                value_type::floating });
            verbs.add_synonym("setsurfacelight", "surfacelight");
            mock_verb("setsurfacemat", value_type::nothing, { value_type::surface,
                                                              value_type::material });

            // System
            mock_verb("bitclear", value_type::integer, { value_type::integer,
                                                         value_type::integer });
            mock_verb("bitset", value_type::integer, { value_type::integer,
                                                       value_type::integer });
            mock_verb("bittest", value_type::integer, { value_type::integer,
                                                        value_type::integer });
            mock_verb("getmastercog", value_type::cog, { });
            mock_verb("getselfcog", value_type::cog, { });
            mock_verb("heapfree", value_type::nothing, { });
            mock_verb("heapget", value_type::dynamic, { value_type::integer });
            mock_verb("heapnew", value_type::nothing, { value_type::integer });
            mock_verb("heapset", value_type::nothing, { value_type::integer,
                                                        value_type::dynamic });
            mock_verb("loadkeyframe", value_type::keyframe, { value_type::string });
            mock_verb("loadmodel", value_type::model, { value_type::string });
            mock_verb("loadsound", value_type::sound, { value_type::string });
            mock_verb("loadtemplate", value_type::thing_template, { value_type::string });
            mock_verb("rand", value_type::floating, { });
            mock_verb("setmastercog", value_type::nothing, { value_type::cog });

            // Thing action
            mock_verb("attachthingtosurf", value_type::nothing, { value_type::thing,
                                                                  value_type::surface });
            mock_verb("attachthingtothing", value_type::nothing, { value_type::thing,
                                                                   value_type::thing });
            mock_verb("attachthingtothingex", value_type::nothing, { value_type::thing,
                                                                     value_type::thing,
                                                                     value_type::integer });
            mock_verb("capturething", value_type::nothing, { value_type::thing });
            mock_verb("creatething", value_type::thing, { value_type::thing_template,
                                                          value_type::thing });
            mock_verb("createthingatpos", value_type::thing, { value_type::thing_template,
                                                               value_type::sector,
                                                               value_type::vector,
                                                               value_type::vector });
            mock_verb("createthingatposnr", value_type::thing, { value_type::thing_template,
                                                                 value_type::sector,
                                                                 value_type::vector,
                                                                 value_type::vector });
            mock_verb("createthingnr", value_type::thing, { value_type::thing_template,
                                                            value_type::thing });
            mock_verb("damagething", value_type::floating, { value_type::thing,
                                                             value_type::floating,
                                                             value_type::integer,
                                                             value_type::thing });
            mock_verb("destroything", value_type::nothing, { value_type::thing });
            mock_verb("detachthing", value_type::nothing, { value_type::thing });
            mock_verb("firstthinginview", value_type::thing, { value_type::thing,
                                                               value_type::floating,
                                                               value_type::floating,
                                                               value_type::integer });
            mock_verb("getthinglvec", value_type::vector, { value_type::thing });
            mock_verb("getthingpos", value_type::vector, { value_type::thing });
            mock_verb("getthingrvec", value_type::vector, { value_type::thing });
            mock_verb("getthinguvec", value_type::vector, { value_type::thing });
            mock_verb("ismoving", value_type::boolean, { value_type::thing });
            mock_verb("isthingmoving", value_type::boolean, { value_type::thing });
            mock_verb("isthingvisible", value_type::boolean, { value_type::thing });
            mock_verb("nextthinginview", value_type::thing, { });
            mock_verb("releasething", value_type::nothing, { value_type::thing });
            mock_verb("setthinglook", value_type::nothing, { value_type::thing,
                                                             value_type::vector });
            mock_verb("setthingpos", value_type::nothing, { value_type::thing,
                                                            value_type::vector });
            mock_verb("teleportthing", value_type::nothing, { value_type::thing,
                                                              value_type::thing });
            mock_verb("thinglightanim", value_type::thing, { value_type::thing,
                                                             value_type::floating,
                                                             value_type::floating,
                                                             value_type::floating });

            // Thing flags
            mock_verb("clearphysicsflags", value_type::nothing, { value_type::thing,
                                                                  value_type::integer });
            mock_verb("clearthingattachflags", value_type::nothing, { value_type::thing,
                                                                      value_type::integer });
            mock_verb("clearthingflags", value_type::nothing, { value_type::thing,
                                                                value_type::integer });
            mock_verb("cleartypeflags", value_type::nothing, { value_type::thing,
                                                               value_type::integer });
            verbs.add_synonym("cleartypeflags", "clearactorflags");
            verbs.add_synonym("cleartypeflags", "clearexplosionflags");
            verbs.add_synonym("cleartypeflags", "clearitemflags");
            verbs.add_synonym("cleartypeflags", "clearparticleflags");
            verbs.add_synonym("cleartypeflags", "clearweaponflags");
            mock_verb("getphysicsflags", value_type::integer, { value_type::thing });
            mock_verb("getthingattachflags", value_type::integer, { value_type::thing });
            verbs.add_synonym("getthingattachflags", "getattachflags");
            mock_verb("getthingflags", value_type::integer, { value_type::thing });
            mock_verb("gettypeflags", value_type::integer, { value_type::thing });
            verbs.add_synonym("gettypeflags", "getactorflags");
            verbs.add_synonym("gettypeflags", "getexplosionflags");
            verbs.add_synonym("gettypeflags", "getitemflags");
            verbs.add_synonym("gettypeflags", "getparticleflags");
            verbs.add_synonym("gettypeflags", "getweaponflags");
            mock_verb("setphysicsflags", value_type::nothing, { value_type::thing,
                                                                value_type::integer });
            mock_verb("setthingattachflags", value_type::nothing, { value_type::thing,
                                                                    value_type::integer });
            mock_verb("setthingflags", value_type::nothing, { value_type::thing,
                                                              value_type::integer });
            mock_verb("settypeflags", value_type::nothing, { value_type::thing,
                                                             value_type::integer });
            verbs.add_synonym("settypeflags", "setactorflags");
            verbs.add_synonym("settypeflags", "setexplosionflags");
            verbs.add_synonym("settypeflags", "setitemflags");
            verbs.add_synonym("settypeflags", "setparticleflags");
            verbs.add_synonym("settypeflags", "setweaponflags");

            // Thing mode
            mock_verb("getthingcurgeomode", value_type::integer, { value_type::thing });
            mock_verb("getthingcurlightmode", value_type::integer, { value_type::thing });
            mock_verb("getthingcurtexmode", value_type::integer, { value_type::thing });
            mock_verb("getthinggeomode", value_type::integer, { value_type::thing });
            mock_verb("getthinglightmode", value_type::integer, { value_type::thing });
            mock_verb("getthingtexmode", value_type::integer, { value_type::thing });
            mock_verb("setthingcurgeomode", value_type::nothing, { value_type::thing,
                                                                   value_type::integer });
            mock_verb("setthingcurlightmode", value_type::nothing, { value_type::thing,
                                                                     value_type::integer });
            mock_verb("setthingcurtexmode", value_type::nothing, { value_type::thing,
                                                                   value_type::integer });
            mock_verb("setthinggeomode", value_type::nothing, { value_type::thing,
                                                                value_type::integer });
            mock_verb("setthinglightmode", value_type::nothing, { value_type::thing,
                                                                  value_type::integer });
            mock_verb("setthingtexmode", value_type::nothing, { value_type::thing,
                                                                value_type::integer });

            // Thing property
            mock_verb("getcollidetype", value_type::integer, { value_type::thing });
            mock_verb("getlifeleft", value_type::floating, { value_type::thing });
            mock_verb("getthingcapturecog", value_type::cog, { value_type::thing });
            mock_verb("getthingclasscog", value_type::cog, { value_type::thing });
            mock_verb("getthingcollidesize", value_type::floating, { value_type::thing });
            mock_verb("getthingfireoffset", value_type::vector, { value_type::thing });
            mock_verb("getthinglight", value_type::floating, { value_type::thing });
            mock_verb("getthingmass", value_type::floating, { value_type::thing });
            mock_verb("getthingmodel", value_type::model, { value_type::thing });
            mock_verb("getthingmovesize", value_type::floating, { value_type::thing });
            mock_verb("getthingparent", value_type::thing, { value_type::thing });
            mock_verb("getthingrespawn", value_type::floating, { value_type::thing });
            mock_verb("getthingsector", value_type::sector, { value_type::thing });
            mock_verb("getthingsignature", value_type::integer, { value_type::thing });
            mock_verb("getthingtemplate", value_type::thing_template, { value_type::thing });
            mock_verb("getthingtype", value_type::integer, { value_type::thing });
            mock_verb("getthinguserdata", value_type::dynamic, { value_type::thing });
            mock_verb("setcollidetype", value_type::nothing, { value_type::thing,
                                                               value_type::integer });
            mock_verb("setlifeleft", value_type::nothing, { value_type::thing,
                                                            value_type::floating });
            mock_verb("setthingcapturecog", value_type::nothing, { value_type::thing,
                                                                   value_type::cog });
            mock_verb("setthingclasscog", value_type::nothing, { value_type::thing,
                                                                 value_type::cog });
            mock_verb("setthingcollidesize", value_type::nothing, { value_type::thing,
                                                                    value_type::floating });
            mock_verb("setthingfireoffset", value_type::nothing, { value_type::thing,
                                                                   value_type::vector });
            mock_verb("setthinglight", value_type::nothing, { value_type::thing,
                                                              value_type::floating,
                                                              value_type::floating });
            verbs.add_synonym("setthinglight", "thinglight");
            mock_verb("setthingmass", value_type::nothing, { value_type::thing,
                                                             value_type::floating });
            mock_verb("setthingmodel", value_type::nothing, { value_type::thing,
                                                              value_type::model });
            mock_verb("setthingmovesize", value_type::nothing, { value_type::thing,
                                                                 value_type::floating });
            mock_verb("setthingtype", value_type::nothing, { value_type::thing,
                                                             value_type::integer });
            mock_verb("setthinguserdata", value_type::nothing, { value_type::thing,
                                                                 value_type::floating });

            // Vector
            mock_verb("randvec", value_type::vector, { });
            mock_verb("vectoradd", value_type::vector, { value_type::vector,
                                                         value_type::vector });
            mock_verb("vectorcross", value_type::vector, { value_type::vector,
                                                           value_type::vector });
            mock_verb("vectordist", value_type::floating, { value_type::vector,
                                                            value_type::vector });
            mock_verb("vectordot", value_type::floating, { value_type::vector,
                                                           value_type::vector });
            mock_verb("vectorlen", value_type::floating, { value_type::vector });
            mock_verb("vectornorm", value_type::vector, { value_type::vector });
            mock_verb("vectorscale", value_type::vector, { value_type::vector,
                                                           value_type::floating });
            mock_verb("vectorset", value_type::vector, { value_type::floating,
                                                         value_type::floating,
                                                         value_type::floating });
            mock_verb("vectorsub", value_type::vector, { value_type::vector,
                                                         value_type::vector });
            mock_verb("vectorx", value_type::floating, { value_type::vector });
            mock_verb("vectory", value_type::floating, { value_type::vector });
            mock_verb("vectorz", value_type::floating, { value_type::vector });

            // Velocity
            mock_verb("addthingvel", value_type::nothing, { value_type::thing,
                                                            value_type::vector });
            mock_verb("applyforce", value_type::nothing, { value_type::thing,
                                                           value_type::vector });
            mock_verb("getthingrotvel", value_type::vector, { value_type::thing });
            mock_verb("getthingthrust", value_type::vector, { value_type::thing });
            mock_verb("getthingvel", value_type::vector, { value_type::thing });
            mock_verb("setthingrotvel", value_type::nothing, { value_type::thing,
                                                               value_type::vector });
            mock_verb("setthingthrust", value_type::nothing, { value_type::thing,
                                                               value_type::vector });
            mock_verb("setthingvel", value_type::nothing, { value_type::thing,
                                                            value_type::vector });
            mock_verb("stopthing", value_type::nothing, { value_type::thing });

            // Weapon
            mock_verb("activatebin", value_type::nothing, { value_type::thing,
                                                            value_type::floating,
                                                            value_type::integer });
            mock_verb("activateweapon", value_type::nothing, { value_type::thing,
                                                               value_type::floating,
                                                               value_type::integer });
            mock_verb("assignweapon", value_type::nothing, { value_type::thing,
                                                             value_type::integer });
            mock_verb("autoselectweapon", value_type::integer, { value_type::thing,
                                                                 value_type::integer });
            mock_verb("changefirerate", value_type::nothing, { value_type::thing,
                                                               value_type::floating });
            mock_verb("deactivatebin", value_type::floating, { value_type::thing,
                                                               value_type::integer });
            mock_verb("deactivateweapon", value_type::floating, { value_type::thing,
                                                                  value_type::integer });
            mock_verb("fireprojectile", value_type::thing, { value_type::thing,
                                                             value_type::thing_template,
                                                             value_type::sound,
                                                             value_type::integer,
                                                             value_type::vector,
                                                             value_type::vector,
                                                             value_type::floating,
                                                             value_type::integer,
                                                             value_type::floating,
                                                             value_type::floating });
            mock_verb("getcurweapon", value_type::integer, { value_type::thing });
            verbs.add_synonym("getcurweapon", "getcurinvweapon");
            mock_verb("getcurweaponmode", value_type::integer, { });
            mock_verb("getweaponpriority", value_type::floating, { value_type::thing,
                                                                   value_type::integer,
                                                                   value_type::integer });
            mock_verb("jksetpovmodel", value_type::nothing, { value_type::thing,
                                                              value_type::model });
            mock_verb("jksetwaggle", value_type::nothing, { value_type::thing,
                                                            value_type::vector,
                                                            value_type::floating });
            mock_verb("jksetweaponmesh", value_type::nothing, { value_type::thing,
                                                                value_type::model });
            mock_verb("selectweapon", value_type::nothing, { value_type::thing,
                                                             value_type::integer });
            mock_verb("setarmedmode", value_type::nothing, { value_type::thing,
                                                             value_type::integer });
            mock_verb("setcurinvweapon", value_type::nothing, { value_type::thing,
                                                                value_type::integer });
            mock_verb("setcurweapon", value_type::nothing, { value_type::thing,
                                                             value_type::integer });
            mock_verb("setfirewait", value_type::nothing, { value_type::thing,
                                                            value_type::floating });
            mock_verb("setmountwait", value_type::nothing, { value_type::thing,
                                                             value_type::floating });

            // Gorc will not support the following verbs. They are not used in the original
            // game and they impose unreasonable implementation limits.
            // Cogcheck will display a warning if these verbs are used. Gorc itself will fail
            // to compile any cog that uses them.
            verbs.add_deprecation("heapfree");
            verbs.add_deprecation("heapget");
            verbs.add_deprecation("heapnew");
            verbs.add_deprecation("heapset");
            verbs.add_deprecation("setthingtype");
            verbs.add_deprecation("parsearg");
        }
    };

}

MAKE_MAIN(gorc::cog_check_program)
