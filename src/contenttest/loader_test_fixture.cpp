#include "loader_test_fixture.h"
#include <fstream>

LoaderTestFixture::LoaderTestFixture(const boost::filesystem::path& BasePath)
	: nfs(BasePath), FileSystem(nfs), ContentManager(Report, FileSystem), Compiler(VerbTable) {
	CreateMockCogVerbs();
	return;
}

LoaderTestFixture::LoaderTestFixture(const Gorc::Content::FileSystem& fs)
	: nfs(""), FileSystem(fs), ContentManager(Report, FileSystem), Compiler(VerbTable) {
	CreateMockCogVerbs();
	return;
}

void LoaderTestFixture::CreateMockCogVerbs() {
	// Macros for building mock verbs
#define V_PARAM_0
#define V_PARAM_1 int
#define V_PARAM_2 int, V_PARAM_1
#define V_PARAM_3 int, V_PARAM_2
#define V_PARAM_4 int, V_PARAM_3
#define V_PARAM_5 int, V_PARAM_4
#define V_PARAM_6 int, V_PARAM_5
#define V_PARAM_7 int, V_PARAM_6
#define V_PARAM_8 int, V_PARAM_7
#define V_PARAM_9 int, V_PARAM_8
#define V_PARAM_10 int, V_PARAM_9

#define VERB_VOID(x, y) VerbTable.AddVerb<void, y>(x, [](V_PARAM_##y){});
#define VERB_INT(x, y) VerbTable.AddVerb<int, y>(x, [](V_PARAM_##y) -> int { return 0; });
#define VERB_BOOL(x, y) VerbTable.AddVerb<bool, y>(x, [](V_PARAM_##y) -> bool { return false; });

#define CONST(x, y) ConstantTable.insert(std::make_pair(x, Gorc::Cog::VM::Value(static_cast<int>(y))));
	// Build mock verb table for characteristic testing.

	VERB_VOID("aiclearmode", 2);
	VERB_VOID("aiflee", 2);
	VERB_INT("aigetmode", 1);
	VERB_VOID("aijump", 3);
	VERB_VOID("aisetclass", 2);
	VERB_VOID("aisetfiretarget", 2);
	VERB_VOID("aisetlookpos", 2);
	VERB_VOID("aisetmode", 2);
	VERB_VOID("aisetmovepos", 2);
	VERB_VOID("aisetmovespeed", 2);
	VERB_VOID("aisetmovething", 2);
	VERB_INT("getactorweapon", 2);
	VERB_BOOL("isaitargetinsight", 1);
	VERB_VOID("setactorweapon", 3);

	VERB_INT("getmaterialcel", 1);
	VERB_INT("materialanim", 3);
	VERB_VOID("setmaterialcel", 2);
	VERB_INT("getsurfaceanim", 1);
	VERB_INT("getsurfacecel", 1);
	VERB_INT("getwallcel", 1);
	VERB_INT("setsurfacecel", 2);		// Returns the value of the second parameter (?)
	VERB_INT("setwallcel", 2);			// Returns the value of the second parameter (?)
	VERB_VOID("stopsurfaceanim", 1);
	VERB_INT("surfaceanim", 3);
	VERB_INT("slideceilingsky", 2);
	VERB_INT("slidehorizonsky", 2);
	VERB_INT("slidesurface", 3);
	VERB_INT("slidewall", 3);
	VERB_VOID("stopanim", 1);
	VERB_INT("surfacelightanim", 4);

	VERB_VOID("cyclecamera", 0);
	VERB_INT("getcamerastateflags", 0);
	VERB_INT("getcurrentcamera", 0);
	VERB_INT("getprimaryfocus", 1);
	VERB_VOID("setcamerafocus", 2);
	VERB_VOID("setcamerastateflags", 1);
	VERB_VOID("setcurrentcamera", 1);
	VERB_VOID("setpovshake", 4);

	VERB_VOID("adddynamicadd", 4);
	VERB_VOID("adddynamictint", 4);
	VERB_VOID("freecoloreffect", 1);
	VERB_INT("newcoloreffect", 10);

	VERB_INT("getheadlightintensity", 1);
	VERB_INT("gethealth", 1);
	VERB_INT("getthinghealth", 1);
	VERB_BOOL("haslos", 2);
	VERB_VOID("healthing", 2);
	VERB_BOOL("iscrouching", 1);
	VERB_BOOL("isthingcrouching", 1);
	VERB_VOID("jkclearflags", 2);
	VERB_INT("jkgetflags", 1);
	VERB_VOID("jksetflags", 2);
	VERB_VOID("jksetinvis", 2);
	VERB_VOID("jksetinvulnerable", 2);
	VERB_VOID("setactorextraspeed", 2);
	VERB_VOID("setheadlightintensity", 2);
	VERB_VOID("sethealth", 2);
	VERB_VOID("setthinghealth", 2);

	VERB_VOID("aisetlookframe", 2);
	VERB_VOID("aisetmoveframe", 2);
	VERB_INT("getcurframe", 1);
	VERB_INT("getgoalframe", 1);
	VERB_VOID("jumptoframe", 3);
	VERB_VOID("movetoframe", 3);
	VERB_VOID("pathmovepause", 1);
	VERB_VOID("pathmoveresume", 1);
	VERB_VOID("skiptoframe", 3);
	VERB_VOID("waitforstop", 1);
	VERB_VOID("rotate", 2);
	VERB_VOID("rotatepivot", 3);

	VERB_VOID("changeinv", 3);
	VERB_INT("getinv", 2);
	VERB_INT("getinvcog", 2);
	VERB_INT("getinvmax", 2);
	VERB_INT("getinvmin", 2);
	VERB_BOOL("isinvactivated", 2);
	VERB_BOOL("isinvavailable", 2);
	VERB_VOID("setbinwait", 3);
	VERB_VOID("setinv", 3);
	VERB_VOID("setinvactivated", 3);
	VERB_VOID("setinvavailable", 3);
	VERB_VOID("setinvflags", 3);

	VERB_INT("getkeylen", 1);
	VERB_INT("getmajormode", 1);
	VERB_INT("jkplaypovkey", 4);
	VERB_VOID("jkstoppovkey", 3);
	VERB_INT("playkey", 4);
	VERB_INT("playmode", 2);
	VERB_VOID("stopkey", 3);

	VERB_VOID("autosavegame", 0);
	VERB_INT("getceilingskyoffset", 0);
	VERB_INT("getflexgametime", 0);
	VERB_INT("getgametime", 0);
	VERB_INT("getgravity", 0);
	VERB_INT("gethorizonskyoffset", 0);
	VERB_INT("getleveltime", 0);
	VERB_INT("getsectorcount", 0);
	VERB_INT("getsurfacecount", 0);
	VERB_INT("getthingcount", 0);
	VERB_INT("getthingtemplatecount", 1);
	VERB_VOID("jkendlevel", 1);
	VERB_VOID("setceilingskyoffset", 2);
	VERB_VOID("setgravity", 1);
	VERB_VOID("sethorizonskyoffset", 2);

	VERB_INT("getparam", 1);
	VERB_INT("getsenderid", 0);
	VERB_INT("getsenderref", 0);
	VERB_INT("getsendertype", 0);
	VERB_INT("getsourceref", 0);
	VERB_INT("getsourcetype", 0);
	VERB_VOID("killtimerex", 1);
	VERB_VOID("returnex", 1);
	VERB_VOID("sendmessage", 2);
	VERB_INT("sendmessageex", 6);
	VERB_VOID("sendtrigger", 6);
	VERB_VOID("setparam", 2);
	VERB_VOID("setpulse", 1);
	VERB_VOID("settimer", 1);
	VERB_VOID("settimerex", 4);
	VERB_VOID("sleep", 1);
	VERB_VOID("setthingpulse", 2);
	VERB_VOID("setthingtimer", 2);

	VERB_VOID("amputatejoint", 2);
	VERB_VOID("disableirmode", 0);
	VERB_VOID("enableirmode", 2);
	VERB_VOID("getsithmode", 0);
	VERB_VOID("jkdisablesaber", 1);
	VERB_VOID("jkenablesaber", 4);
	VERB_VOID("jkendtarget", 0);
	VERB_VOID("jksetpersuasioninfo", 3);
	VERB_VOID("jksetsaberinfo", 9);
	VERB_VOID("jksettarget", 1);
	VERB_VOID("jksettargetcolors", 3);
	VERB_VOID("parsearg", 2);
	VERB_INT("skilltarget", 4);
	VERB_VOID("takeitem", 2);
	VERB_INT("thingviewdot", 2);

	VERB_VOID("clearmultimodeflags", 1);
	VERB_INT("createbackpack", 1);
	VERB_INT("getabsolutemaxplayers", 0);
	VERB_INT("getmaxplayers", 0);
	VERB_INT("getmultimodeflags", 0);
	VERB_INT("getnumplayers", 0);
	VERB_INT("getnumplayersinteam", 1);
	VERB_INT("getplayerteam", 1);
	VERB_INT("getrespawnmask", 1);
	VERB_INT("gettimelimit", 0);
	VERB_BOOL("ismulti", 0);
	VERB_BOOL("isserver", 0);
	VERB_INT("nthbackpackbin", 2);
	VERB_INT("nthbackpackvalue", 2);
	VERB_INT("numbackpackitems", 1);
	VERB_INT("pickupbackpack", 2);
	VERB_VOID("setmultimodeflags", 1);
	VERB_VOID("setplayerteam", 2);
	VERB_VOID("setrespawnmask", 2);
	VERB_VOID("settimelimit", 1);
	VERB_VOID("syncscores", 0);
	VERB_VOID("syncsector", 1);
	VERB_VOID("syncsurface", 1);
	VERB_VOID("syncthingattachment", 1);
	VERB_VOID("syncthingpos", 1);
	VERB_VOID("syncthingstate", 1);

	VERB_INT("getautopickup", 0);
	VERB_INT("getautoreload", 0);
	VERB_INT("getautoswitch", 0);
	VERB_INT("getdifficulty", 0);
	VERB_BOOL("jkgetsabercam", 0);
	VERB_VOID("setautopickup", 2);
	VERB_VOID("setautoreload", 2);
	VERB_VOID("setautoswitch", 2);

	VERB_INT("getparticlegrowthspeed", 1);
	VERB_INT("getparticlesize", 1);
	VERB_INT("getparticletimeoutrate", 1);
	VERB_VOID("setparticlegrowthspeed", 2);
	VERB_VOID("setparticlesize", 2);
	VERB_VOID("setparticletimeoutrate", 2);

	VERB_VOID("cleargoalflags", 3);
	VERB_INT("getlocalplayerthing", 0);
	VERB_INT("getplayernum", 1);
	VERB_INT("getplayerthing", 1);
	VERB_VOID("jkclearsuperflags", 1);
	VERB_INT("jkgetchoice", 0);
	VERB_INT("jkgetlocalplayer", 0);
	VERB_INT("jkgetsuperflags", 0);
	VERB_VOID("jksetforcespeed", 1);
	VERB_VOID("jksetsuperflags", 1);
	VERB_VOID("setgoalflags", 3);

	VERB_VOID("jkprintunistring", 2);
	VERB_VOID("jkstringclear", 0);
	VERB_VOID("jkstringconcatasciistring", 1);
	VERB_VOID("jkstringconcatflex", 1);
	VERB_VOID("jkstringconcatformattedflex", 2);
	VERB_VOID("jkstringconcatformattedint", 2);
	VERB_VOID("jkstringconcatint", 1);
	VERB_VOID("jkstringconcatplayername", 1);
	VERB_VOID("jkstringconcatspace", 0);
	VERB_VOID("jkstringconcatunistring", 1);
	VERB_VOID("jkstringconcatvector", 1);
	VERB_VOID("jkstringoutput", 2);
	VERB_VOID("print", 1);
	VERB_VOID("printflex", 1);
	VERB_VOID("printint", 1);
	VERB_VOID("printvector", 1);

	VERB_VOID("addscoretoteammembers", 2);
	VERB_INT("getplayerkilled", 1);
	VERB_INT("getplayerkills", 1);
	VERB_INT("getplayerscore", 1);
	VERB_INT("getplayersuicides", 1);
	VERB_INT("getscorelimit", 0);
	VERB_INT("getteamscore", 1);
	VERB_VOID("setplayerkilled", 2);
	VERB_VOID("setplayerkills", 2);
	VERB_VOID("setplayerscore", 2);
	VERB_VOID("setplayersuicides", 2);
	VERB_VOID("setscorelimit", 1);
	VERB_VOID("setteamscore", 2);

	VERB_VOID("clearsectorflags", 2);
	VERB_INT("firstthinginsector", 1);
	VERB_INT("getcolormap", 1);
	VERB_INT("getnumsectorsurfaces", 1);
	VERB_INT("getnumsectorvertices", 1);
	VERB_INT("getsectorcenter", 1);
	VERB_INT("getsectorcolormap", 1);
	VERB_INT("getsectorflags", 1);
	VERB_INT("getsectorlight", 1);
	VERB_INT("getsectorplayercount", 1);
	VERB_INT("getsectorsurfaceref", 2);
	VERB_INT("getsectorthingcount", 1);
	VERB_INT("getsectorthrust", 1);
	VERB_INT("getsectortint", 1);
	VERB_INT("getsectorvertexpos", 2);
	VERB_INT("nextthinginsector", 1);
	VERB_INT("prevthinginsector", 1);
	VERB_VOID("sectoradjoins", 2);
	VERB_VOID("sectorlight", 3);
	VERB_INT("sectorplayercount", 1);
	VERB_INT("sectorthingcount", 1);
	VERB_VOID("sectorthrust", 3);
	VERB_VOID("setcolormap", 2);
	VERB_VOID("setsectoradjoins", 2);
	VERB_VOID("setsectorcolormap", 2);
	VERB_VOID("setsectorflags", 2);
	VERB_VOID("setsectorlight", 3);
	VERB_VOID("setsectorthrust", 3);
	VERB_VOID("setsectortint", 2);

	VERB_VOID("changesoundpitch", 3);
	VERB_VOID("changesoundvol", 3);
	VERB_INT("getsoundlen", 1);
	VERB_VOID("playsong", 3);
	VERB_INT("playsoundclass", 2);
	VERB_INT("playsoundglobal", 4);
	VERB_INT("playsoundlocal", 4);
	VERB_INT("playsoundpos", 6);
	VERB_INT("playsoundthing", 6);
	VERB_VOID("sectorsound", 3);
	VERB_VOID("setmusicvol", 1);
	VERB_VOID("stopsound", 2);

	VERB_VOID("cleardebugmodeflags", 1);
	VERB_VOID("clearmapmodeflags", 1);
	VERB_VOID("clearsubmodeflags", 1);
	VERB_INT("getdebugmodeflags", 0);
	VERB_INT("getmapmodeflags", 0);
	VERB_INT("getsubmodeflags", 0);
	VERB_VOID("setdebugmodeflags", 1);
	VERB_VOID("setmapmodeflags", 1);
	VERB_VOID("setsubmodeflags", 1);

	VERB_VOID("clearadjoinflags", 2);
	VERB_VOID("clearfacetype", 2);
	VERB_VOID("clearsurfaceflags", 2);
	VERB_INT("getadjoinflags", 1);
	VERB_INT("getfacegeomode", 1);
	VERB_INT("getfacelightmode", 1);
	VERB_INT("getfacetexmode", 1);
	VERB_INT("getfacetype", 1);
	VERB_INT("getnumsurfacevertices", 1);
	VERB_INT("getsurfaceadjoin", 1);
	VERB_INT("getsurfacecenter", 1);
	VERB_INT("getsurfaceflags", 1);
	VERB_INT("getsurfacelight", 1);
	VERB_INT("getsurfacemat", 1);
	VERB_INT("getsurfacenormal", 1);
	VERB_INT("getsurfacesector", 1);
	VERB_INT("getsurfacevertexpos", 1);
	VERB_VOID("setadjoinflags", 2);
	VERB_VOID("setfacegeomode", 2);
	VERB_VOID("setfacelightmode", 2);
	VERB_VOID("setfacetexmode", 2);
	VERB_VOID("setfacetype", 2);
	VERB_VOID("setsurfaceflags", 2);
	VERB_VOID("setsurfacelight", 3);
	VERB_VOID("setsurfacemat", 2);
	VERB_INT("surfacecenter", 1);
	VERB_VOID("surfacelight", 3);

	VERB_INT("bitclear", 2);
	VERB_INT("bitset", 2);
	VERB_INT("bittest", 2);
	VERB_INT("getmastercog", 0);
	VERB_INT("getselfcog", 0);
	VERB_VOID("heapfree", 0);
	VERB_INT("heapget", 1);
	VERB_VOID("heapnew", 1);
	VERB_VOID("heapset", 2);
	VERB_INT("loadkeyframe", 1);
	VERB_INT("loadmodel", 1);
	VERB_INT("loadsound", 1);
	VERB_INT("loadtemplate", 1);
	VERB_INT("rand", 0);
	VERB_VOID("reset", 0);
	VERB_VOID("setmastercog", 1);

	VERB_VOID("attachthingtosurf", 2);
	VERB_VOID("attachthingtothing", 2);
	VERB_VOID("attachthingtothingex", 3);
	VERB_VOID("capturething", 1);
	VERB_INT("creatething", 2);
	VERB_INT("createthingatpos", 4);
	VERB_INT("createthingatposnr", 4);
	VERB_INT("createthingnr", 2);
	VERB_INT("damagething", 4);
	VERB_VOID("destroything", 1);
	VERB_VOID("detachthing", 1);
	VERB_INT("firstthinginview", 4);
	VERB_INT("getthinglvec", 1);
	VERB_INT("getthingpos", 1);
	VERB_INT("getthingrvec", 1);
	VERB_INT("getthinguvec", 1);
	VERB_BOOL("ismoving", 1);
	VERB_BOOL("isthingmoving", 1);
	VERB_BOOL("isthingvisible", 1);
	VERB_INT("nextthinginview", 0);
	VERB_VOID("releasething", 1);
	VERB_VOID("setthinglook", 2);
	VERB_VOID("setthingpos", 2);
	VERB_VOID("teleportthing", 2);
	VERB_INT("thinglightanim", 4);

	VERB_VOID("clearactorflags", 2);
	VERB_VOID("clearexplosionflags", 2);
	VERB_VOID("clearitemflags", 2);
	VERB_VOID("clearparticleflags", 2);
	VERB_VOID("clearphysicsflags", 2);
	VERB_VOID("clearthingattachflags", 2);
	VERB_VOID("clearthingflags", 2);
	VERB_VOID("cleartypeflags", 2);
	VERB_VOID("clearweaponflags", 2);
	VERB_INT("getactorflags", 1);
	VERB_INT("getattachflags", 1);
	VERB_INT("getexplosionflags", 1);
	VERB_INT("getitemflags", 1);
	VERB_INT("getparticleflags", 1);
	VERB_INT("getphysicsflags", 1);
	VERB_INT("getthingattachflags", 1);
	VERB_INT("getthingflags", 1);
	VERB_INT("gettypeflags", 1);
	VERB_INT("getweaponflags", 1);
	VERB_VOID("setactorflags", 2);
	VERB_VOID("setexplosionflags", 2);
	VERB_VOID("setitemflags", 2);
	VERB_VOID("setparticleflags", 2);
	VERB_VOID("setphysicsflags", 2);
	VERB_VOID("setthingattachflags", 2);
	VERB_VOID("setthingflags", 2);
	VERB_VOID("settypeflags", 2);
	VERB_VOID("setweaponflags", 2);

	VERB_INT("getthingcurgeomode", 1);
	VERB_INT("getthingcurlightmode", 1);
	VERB_INT("getthingcurtexmode", 1);
	VERB_INT("getthinggeomode", 1);
	VERB_INT("getthinglightmode", 1);
	VERB_INT("getthingtexmode", 1);
	VERB_VOID("setthingcurgeomode", 2);
	VERB_VOID("setthingcurlightmode", 2);
	VERB_VOID("setthingcurtexmode", 2);
	VERB_VOID("setthinggeomode", 2);
	VERB_VOID("setthinglightmode", 2);
	VERB_VOID("setthingtexmode", 2);

	VERB_INT("getcollidetype", 1);
	VERB_INT("getlifeleft", 1);
	VERB_INT("getthingcapturecog", 1);
	VERB_INT("getthingclasscog", 1);
	VERB_INT("getthingcollidesize", 1);
	VERB_INT("getthingfireoffset", 1);
	VERB_INT("getthinglight", 1);
	VERB_INT("getthingmass", 1);
	VERB_INT("getthingmodel", 1);
	VERB_INT("getthingmovesize", 1);
	VERB_INT("getthingparent", 1);
	VERB_INT("getthingrespawn", 1);
	VERB_INT("getthingsector", 1);
	VERB_INT("getthingsignature", 1);
	VERB_INT("getthingtemplate", 1);
	VERB_INT("getthingtype", 1);
	VERB_INT("getthinguserdata", 1);
	VERB_VOID("setcollidetype", 2);
	VERB_VOID("setlifeleft", 2);
	VERB_VOID("setthingcapturecog", 2);
	VERB_VOID("setthingclasscog", 2);
	VERB_VOID("setthingcollidesize", 2);
	VERB_VOID("setthingfireoffset", 2);
	VERB_VOID("setthinglight", 3);
	VERB_VOID("setthingmass", 2);
	VERB_VOID("setthingmodel", 2);
	VERB_VOID("setthingmovesize", 2);
	VERB_VOID("setthingtype", 2);
	VERB_VOID("setthinguserdata", 2);
	VERB_VOID("thinglight", 3);

	VERB_INT("randvec", 0);
	VERB_INT("vectoradd", 2);
	VERB_INT("vectorcross", 2);
	VERB_INT("vectordist", 2);
	VERB_INT("vectordot", 2);
	VERB_INT("vectorlen", 1);
	VERB_INT("vectornorm", 1);
	VERB_INT("vectorscale", 2);
	VERB_INT("vectorset", 3);
	VERB_INT("vectorsub", 2);
	VERB_INT("vectorx", 1);
	VERB_INT("vectory", 1);
	VERB_INT("vectorz", 1);

	VERB_INT("addthingvel", 2);
	VERB_INT("applyforce", 2);
	VERB_INT("getthingrotvel", 1);
	VERB_INT("getthingthrust", 1);
	VERB_INT("getthingvel", 1);
	VERB_VOID("setthingrotvel", 2);
	VERB_VOID("setthingthrust", 2);
	VERB_VOID("setthingvel", 2);
	VERB_VOID("stopthing", 1);

	VERB_VOID("activatebin", 3);
	VERB_VOID("activateweapon", 3);
	VERB_VOID("assignweapon", 2);
	VERB_INT("autoselectweapon", 2);
	VERB_VOID("changefirerate", 2);
	VERB_INT("deactivatebin", 2);
	VERB_INT("deactivateweapon", 2);
	VERB_INT("fireprojectile", 10);
	VERB_INT("getcurinvweapon", 1);
	VERB_INT("getcurweapon", 1);
	VERB_INT("getcurweaponmode", 0);
	VERB_INT("getweaponpriority", 3);
	VERB_VOID("jksetpovmodel", 2);
	VERB_VOID("jksetwaggle", 3);
	VERB_VOID("jksetweaponmesh", 2);
	VERB_VOID("selectweapon", 2);
	VERB_VOID("setarmedmode", 2);
	VERB_VOID("setcurinvweapon", 2);
	VERB_VOID("setcurweapon", 2);
	VERB_VOID("setfirewait", 2);
	VERB_VOID("setmountwait", 2);
}

void LoaderTestFixture::PrintErrors() const {
	for(const auto& error : Report) {
		NullUnit::Test_Reporter->CaseExpectationFail(
				NullUnit::Test_Suite_Name,
				NullUnit::Test_Case_Name,
				static_cast<std::string>(error),
				error.Location.filename.generic_string(),
				error.Location.first_line);
	}
}
