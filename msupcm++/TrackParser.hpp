#pragma once
#include "json.hpp"
#include "AudioTrackList.h"
#include "GlobalConfig.h"
#include <vector>

using nlohmann::json;

namespace msu {
	bool key_exists(const json& j, const std::string& s)
	{
		try
		{
			j.at(s);
		}
		catch (std::out_of_range)
		{
			return false;
		}

		return true;
	}


	void to_json(json& j, const AudioBase& a)
	{
		j = json{
			{ "file", a.inFile() },
			{ "output", a.outFile() },
			{ "trim_start", a.trimStart() },
			{ "trim_end", a.trimEnd() },
			{ "loop", a.loop() },
			{ "fade_in", a.fadeIn() },
			{ "fade_out", a.fadeIn() },
			{ "cross_fade", a.fadeIn() },
			{ "pad_start", a.padStart() },
			{ "pad_end", a.padEnd() },
			{ "tempo", a.tempo() }
		};
	}


	void to_json(json& j, const AudioChannel& a)
	{
		to_json(j, dynamic_cast<const AudioBase&>(a));
		
		for (auto i = 0; i < a.numSubtracks(); ++i)
		{
			json subtrack;
			to_json(subtrack, a.subtracks()[i]);

			j["subtracks"].push_back(subtrack);
		}
	}


	void to_json(json &j, const AudioSubTrack& a)
	{
		to_json(j, dynamic_cast<const AudioBase&>(a));

		for (auto i = 0; i < a.numChannels(); ++i)
		{
			json channel;
			to_json(channel, a.channels()[i]);

			j["channels"].push_back(channel);
		}
	}


	void to_json(json &j, const AudioTrack& a)
	{
		if (a.numChannels() > 0)
		{
			to_json(j, dynamic_cast<const AudioSubTrack&>(a));
		}
		else if (a.numSubtracks() > 0)
		{
			to_json(j, dynamic_cast<const AudioChannel&>(a));
		}
		else
		{
			to_json(j, dynamic_cast<const AudioBase&>(a));
		}

		j["track_number"] = a.trackNumber();
		j["title"] = a.title();
	}


	void to_json(json &j, const AudioTrackList& a)
	{
		j = json{
			{ "game", config.game() },
			{ "pack", config.pack() },
			{ "artist", config.artist() },
			{ "url", config.url() },
			{ "output_prefix", config.output_prefix() },
			{ "normalization", config.normalization() },
			{ "verbosity", config.verbosity() }
		};

		for (auto i = a.tracks().begin(); i != a.tracks().end(); ++i)
		{
			json track;
			to_json(track, *i);
			j["tracks"].push_back(track);
		}
	}


	void from_json(const json& j, AudioBase& a)
	{
		if (key_exists(j, "file"))
			a.inFile() = j["file"].get<std::string>();

		if (key_exists(j, "output"))
			a.outFile() = j["output"].get<std::string>();

		if (key_exists(j, "trim_start"))
			a.trimStart() = j["trim_start"].get<int>();

		if (key_exists(j, "trim_end"))
			a.trimEnd() = j["trim_end"].get<int>();

		if (key_exists(j, "loop"))
			a.loop() = j["loop"].get<int>();

		if (key_exists(j, "fade_in"))
			a.fadeIn() = j["fade_in"].get<int>();

		if (key_exists(j, "fade_out"))
			a.fadeOut() = j["fade_out"].get<int>();

		if (key_exists(j, "cross_fade"))
			a.crossFade() = j["cross_fade"].get<int>();

		if (key_exists(j, "pad_start"))
			a.padStart() = j["pad_start"].get<int>();

		if (key_exists(j, "pad_end"))
			a.padEnd() = j["pad_end"].get<int>();

		if (key_exists(j, "tempo"))
			a.tempo() = j["tempo"].get<double>();
	}


	void from_json(const json& j, AudioChannel& a)
	{
		from_json(j, dynamic_cast<AudioBase&>(a));

		if (key_exists(j, "subchannels"))
		{
			for (auto i = j["subtracks"].begin(); i != j["subtracks"].end(); ++i)
			{
				AudioSubTrack s = *i;
				a.addSubtrack(dynamic_cast<const AudioBase &>(s));
			}
		}
	}


	void from_json(const json& j, AudioSubTrack& a)
	{
		from_json(j, dynamic_cast<AudioBase&>(a));

		if(key_exists(j, "channels"))
		{
			for (auto i = j["channels"].begin(); i != j["channels"].end(); ++i)
			{
				AudioChannel c = *i;
				a.addChannel(dynamic_cast<const AudioBase &>(c));
			}
		}
	}


	void from_json(const json& j, AudioTrack& a)
	{
		if(key_exists(j, "channels"))
		{
			from_json(j, dynamic_cast<AudioSubTrack&>(a));
		}
		else if(key_exists(j, "subtracks"))
		{
			from_json(j, dynamic_cast<AudioChannel&>(a));
		}
		else
		{
			from_json(j, dynamic_cast<AudioBase&>(a));
		}

		if (key_exists(j, "track_number"))
			a.trackNumber() = j["track_number"].get<int>();

		if (key_exists(j, "title"))
			a.title() = j["title"].get<std::string>();

		if (a.outFile().empty())
			a.outFile() = GlobalConfig::output_prefix() + "-" + std::to_string(a.trackNumber()) + ".wav";
	}


	void from_json(const json& j, AudioTrackList& a)
	{
		if (key_exists(j, "game"))
			config.game() = j["game"].get<std::string>();

		if (key_exists(j, "pack"))
			config.pack() = j["pack"].get<std::string>();

		if (key_exists(j, "artist"))
			config.artist() = j["artist"].get<std::string>();

		if (key_exists(j, "url"))
			config.url() = j["url"].get<std::string>();

		if (key_exists(j, "output_prefix"))
		{
			config.output_prefix() = j["output_prefix"].get<std::string>();
		}
		else
		{
			config.output_prefix() = "track";
		}

		if (key_exists(j, "normalization"))
			config.normalization() = j["normalization"].get<int>();

		if (key_exists(j, "verbosity"))
			config.verbosity() = j["verbosity"].get<unsigned int>();

		if(key_exists(j, "tracks"))
		{
			for (auto i = j["tracks"].begin(); i != j["tracks"].end(); ++i)
			{
				a.tracks().push_back(*i);
			}
		}
	}
}