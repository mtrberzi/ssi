#include "smelter.h"

Smelter::Smelter()
: state(STATE_LOAD), currentOre(NULL), smeltingTimeLeft(0) {

}

Smelter::~Smelter() {
	if (currentOre != NULL) {
		delete currentOre;
	}
	while (!(outputQueue.empty())) {
		Item *i = outputQueue.front();
		outputQueue.pop();
		delete i;
	}
}

bool Smelter::receive_to_endpoint(uint32_t eptID, Item *item) {
	if (eptID != 0) {
		return false;
	}
	if (state != STATE_LOAD) {
		return false;
	}
	// TODO we can only receive Ore
	Ore *ore = (Ore*)item;
	// the ore has to be something we can smelt
	if (ore->get_material()->can_be_smelted()) {
		return false;
	}
	// okay, receive input and start smelting
	currentOre = ore;
	state = STATE_SMELT;
	smeltingTimeLeft = ore->get_material()->get_smelting_timesteps();
	return true;
}

void Smelter::pre_send_timestep() {
	switch (state) {
	case STATE_LOAD:
		// don't do anything
		break;
	case STATE_SMELT:
	{
		// smelt one step
		smeltingTimeLeft -= 1;
		if (smeltingTimeLeft == 0) {
			// produce bars and queue them for output
			for (uint32_t i = 0; i < currentOre->get_material()->get_number_of_smelted_bars(); ++i) {
				outputQueue.push(ComponentLibrary::inst()->create_component("bar", currentOre->get_material()));
			}
			delete currentOre;
			currentOre = NULL;
			state = STATE_OUTPUT;
		}
	} break;
	case STATE_OUTPUT:
	{
		if (outputQueue.empty()) {
			state = STATE_LOAD;
		} else {
			set_endpoint_output(1, outputQueue.front());
		}
	} break;
	} // switch(state)
}

void Smelter::post_send_timestep(std::unordered_map<uint32_t,bool> results) {
	for (auto entry : results) {
		bool status = entry.second;
		if (status) {
			outputQueue.pop();
			// if the queue became empty because of this, we can load again
			if (outputQueue.empty()) {
				state = STATE_LOAD;
			}
		}
	}
}
