#include <nullunit/nullunit.h>
#include <memory>
#include <fstream>

#include "cogtest/language/syntactic_test_fixture.h"
#include "vfssingleton.h"

class CharacteristicSyntacticTestFixture : public SyntacticTestFixture {
public:
	CharacteristicSyntacticTestFixture()
		: SyntacticTestFixture(VfsSingleton::get())
	{
		return;
	}
};

BeginSuiteFixture(CharacteristicSyntacticTest, CharacteristicSyntacticTestFixture);

Case(jk_00_aipatrol) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/00_aipatrol.cog");
	AssertResult(0, 0);
}

Case(jk_00_aishootbarrel) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/00_aishootbarrel.cog");
	AssertResult(0, 0);
}

Case(jk_00_endlevel) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/00_endlevel.cog");
	AssertResult(0, 0);
}

Case(jk_00_lightning) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/00_lightning.cog");
	AssertResult(0, 0);
}

Case(jk_00_simple_elev) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/00_simple_elev.cog");
	AssertResult(0, 0);
}

Case(jk_00_slideceiling) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/00_slideceiling.cog");
	AssertResult(0, 0);
}

Case(jk_00_soundtrack) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/00_soundtrack.cog");
	AssertResult(0, 0);
}

Case(jk_02_cratepuzzle_a) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/02_cratepuzzle_a.cog");
	AssertResult(0, 0);
}

Case(jk_04_britta) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/04_britta.cog");
	AssertResult(0, 0);
}

Case(jk_04_britta2) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/04_britta2.cog");
	AssertResult(0, 0);
}

Case(jk_06_max) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/06_max.cog");
	AssertResult(0, 0);
}

Case(jk_08_tiebomber) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/08_tiebomber.cog");
	AssertResult(0, 0);
}

Case(jk_09_pipedoor2) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/09_pipedoor2.cog");
	AssertResult(0, 0);
}

Case(jk_09_pipedoor_a) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/09_pipedoor_a.cog");
	AssertResult(0, 0);
}

Case(jk_09_pipedoor_c) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/09_pipedoor_c.cog");
	AssertResult(0, 0);
}

Case(jk_10_powercore) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/10_powercore.cog");
	AssertResult(0, 0);
}

Case(jk_12_endlevel) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/12_endlevel.cog");
	AssertResult(0, 0);
}

Case(jk_14_endit) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/14_endit.cog");
	AssertResult(0, 0);
}

Case(jk_16a_timer) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/16a_timer.cog");
	AssertResult(0, 0);
}

Case(jk_18_finalthrust1) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/18_finalthrust1.cog");
	AssertResult(0, 0);
}

Case(jk_18_ventpuzzle) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/18_ventpuzzle.cog");
	AssertResult(0, 0);
}

Case(jk_20_bocsearch) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/20_bocsearch.cog");
	AssertResult(0, 0);
}

Case(jk_21_jerecai) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/21_jerecai.cog");
	AssertResult(0, 0);
}

Case(jk_21_jerecseek) {
	VfsSingleton::SetEpisode("The Force Within");
	ParseFile("cog/21_jerecseek.cog");
	AssertResult(0, 0);
}

Case(jk_actor_bt) {
	ParseFile("cog/actor_bt.cog");
	AssertResult(0, 0);
}

Case(jk_actor_fn) {
	ParseFile("cog/actor_fn.cog");
	AssertResult(0, 0);
}

Case(jk_actor_go) {
	ParseFile("cog/actor_go.cog");
	AssertResult(0, 0);
}

Case(jk_actor_re) {
	ParseFile("cog/actor_re.cog");
	AssertResult(0, 0);
}

Case(jk_actor_s3) {
	ParseFile("cog/actor_s3.cog");
	AssertResult(0, 0);
}

Case(jk_actor_sd) {
	ParseFile("cog/actor_sd.cog");
	AssertResult(0, 0);
}

Case(jk_c1_ctfcallback) {
	VfsSingleton::SetEpisode("Jedi Capture the Flag");
	ParseFile("cog/c1_ctfcallback.cog");
	AssertResult(0, 0);
}

Case(jk_c2_dualforcefield) {
	VfsSingleton::SetEpisode("Jedi Capture the Flag");
	ParseFile("cog/c2_dualforcefield.cog");
	AssertResult(0, 0);
}

Case(jk_c2_simpleforcefield) {
	VfsSingleton::SetEpisode("Jedi Capture the Flag");
	ParseFile("cog/c2_simpleforcefield.cog");
	AssertResult(0, 0);
}

Case(jk_c3_fanspuzzle) {
	VfsSingleton::SetEpisode("Jedi Capture the Flag");
	ParseFile("cog/c3_fanspuzzle.cog");
	AssertResult(0, 0);
}

Case(jk_c3_sasneutral) {
	VfsSingleton::SetEpisode("Jedi Capture the Flag");
	ParseFile("cog/c3_sasneutral.cog");
	AssertResult(0, 0);
}

Case(jk_class_sequencer) {
	ParseFile("cog/class_sequencer.cog");
	AssertResult(0, 0);
}

Case(jk_ctf_main) {
	VfsSingleton::SetEpisode("Jedi Capture the Flag");
	ParseFile("cog/ctf_main.cog");
	AssertResult(0, 0);
}

Case(jk_force_pull) {
	ParseFile("cog/force_pull.cog");
	AssertResult(0, 0);
}

Case(jk_force_throw) {
	ParseFile("cog/force_throw.cog");
	AssertResult(0, 0);
}

Case(jk_item_wrenchblue) {
	ParseFile("cog/item_wrenchblue.cog");
	AssertResult(0, 0);
}

Case(jk_kyle) {
	ParseFile("cog/kyle.cog");
	AssertResult(0, 0);
}

Case(jk_levelsong) {
	ParseFile("cog/levelsong.cog");
	AssertResult(0, 0);
}

Case(jk_pow_backpack) {
	ParseFile("cog/pow_backpack.cog");
	AssertResult(0, 0);
}

Case(jk_pow_battery) {
	ParseFile("cog/pow_battery.cog");
	AssertResult(0, 0);
}

Case(jk_pow_railcharges) {
	ParseFile("cog/pow_railcharges.cog");
	AssertResult(0, 0);
}

Case(jk_weap_saber) {
	ParseFile("cog/weap_saber.cog");
	AssertResult(0, 0);
}

Case(jk_weap_seqcharge) {
	ParseFile("cog/weap_seqcharge.cog");
	AssertResult(0, 0);
}

EndSuite(CharacteristicSyntacticTest);
