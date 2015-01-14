#include "../catch.hpp"
#include "../../main/model/audioclip.h"
#include "../../main/model/project.h"
#include "src/main/datasource.h"

using namespace wreckit;
using namespace std;

class MySampleCounterListener : public SampleCounterListener {
public:
	int sc;

	void operator()(int sampleCounter) {
		sc = sampleCounter;
	}
};

TEST_CASE("SampleTraits", "Test SampleCounterListener override") {
	SampleTraits sampleTraits(48000);
	MySampleCounterListener mySampleCounterListener;
	sampleTraits.addSampleCounterListener(mySampleCounterListener);
	REQUIRE(mySampleCounterListener.sc == 0);
	sampleTraits.addSampleCount(100);
	REQUIRE(mySampleCounterListener.sc == 100);
	sampleTraits.addSampleCount(100);
	REQUIRE(mySampleCounterListener.sc == 200);
}

TEST_CASE("AudioClip", "Construct") {
	AudioClip audioClip("9000", "john woo", "project42", 44100);

	REQUIRE(audioClip.getId() == "9000");
	REQUIRE(audioClip.getName() == "john woo");
	REQUIRE(audioClip.getSampleRate() == 44100);

	for(Attribute* attr : audioClip.getAttributes()) {
		cout << attr->display() << endl;
	}
}

TEST_CASE("AudioClip_1", "Test drop() and create()") {
	AudioClip audioClip;
	audioClip.drop();
	audioClip.create();
}

TEST_CASE("AudioClip_2", "Test save() and load()") {
	AudioClip newAudioClip("ludicrous", "project42", 48000);
	newAudioClip.save();
	string id = newAudioClip.getId();
	REQUIRE_FALSE(id.empty());

	AudioClip loadedAudioClip(id);
	loadedAudioClip.load();
	REQUIRE(loadedAudioClip.getName() == "ludicrous");
	REQUIRE(loadedAudioClip.getSampleRate() == 48000);

	newAudioClip.setName("gibs");
	newAudioClip.save();

	loadedAudioClip.load();
	REQUIRE(loadedAudioClip.getName() == "gibs");
	REQUIRE(loadedAudioClip.getSampleRate() == 48000);
}

TEST_CASE("Entity.toString", "Test toString()") {
	AudioClip audioClip("dipstick", "project42", 22050);
	audioClip.save();

	cout << "Saved AudioClip: " << audioClip.toString() << endl;

	AudioClip audioClip2("", "", 22050);
	cout << "Unsaved AudioClip with no name: " << audioClip2.toString() << endl;
}

TEST_CASE("AudioClip_3", "Test find()") {
	AudioClip newAudioClip("ludicrous", "project42", 48000);
	newAudioClip.save();

	AudioClip searchTemplate("", "", 48000);
	vector<AudioClip> clips48k = searchTemplate.find();
	REQUIRE(clips48k.size() == 2);

	for(AudioClip ac : clips48k) {
		REQUIRE_FALSE(ac.getId().empty());
		REQUIRE_FALSE(ac.getName().empty());
		REQUIRE_FALSE(ac.getSampleRate() == 0);
	}

	cout << clips48k[0].toString() << endl;
	cout << clips48k[1].toString() << endl;
}

TEST_CASE("AudioClip_4", "Test find(All)") {
	AudioClip searchTemplateForAll;
	vector<AudioClip> allClips = searchTemplateForAll.find();
	REQUIRE(allClips.size() == 3);
}

TEST_CASE("ApplicationAndProject", "drop, create and save") {
	ProjectApplication app("wreckit");
	Project project;

	project.drop();
	app.drop();

	app.create();
	project.create();

	project.setName("<unnamed>");
	project.save();

	app.setActiveProjectId(project.getId());
	app.save();
}

TEST_CASE("ApplicationAndProject_2", "find and load") {
	ProjectApplication appTemplate("wreckit");
	vector<ProjectApplication> apps = appTemplate.find();
	REQUIRE(apps.size() == 1);

	Project activeProject(apps[0].getActiveProjectId());
	activeProject.load();

	REQUIRE(activeProject.getName() == "<unnamed>");
}

TEST_CASE("ApplicationAndProject_3", "Test find() all") {
	for(int i = 1; i < 6; i++) {
		Project p("", to_string(i * 10), "some_audioclip_id");
		p.save();
	}

	Project t;
	vector<Project> allProjects = t.find();

	REQUIRE(allProjects.size() == 6);

	for(Project proj : allProjects) {
		cout << proj.toString() << endl;
	}
}
