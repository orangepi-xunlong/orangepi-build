rename_audiojack_rule = {
matches = {
{
{ "api.alsa.card.name", "equals", "Starfive-PWMDAC-Sound-Card" },
},
},
apply_properties = {
["node.description"] = "Audio Jack",
},
}
rename_hdmi_audio_rule = {
matches = {
{
{"api.alsa.card.name", "equals", "Starfive-HDMI-Sound-Card"},
},
},
apply_properties = {
["node.description"] = "HDMI Audio",
},
}
rename_wm8960_rule = {
matches = {
{
{"api.alsa.card.name", "equals", "Starfive-WM8960-Sound-Card"},
},
},
apply_properties = {
["node.description"] = "WM8960 Audio",
},
}
table.insert(alsa_monitor.rules, rename_wm8960_rule)
table.insert(alsa_monitor.rules, rename_audiojack_rule)
table.insert(alsa_monitor.rules, rename_hdmi_audio_rule)
rename_audiojack_rule = {
matches = {
{
{ "api.alsa.card.name", "equals", "Starfive-PWMDAC-Sound-Card" },
},
},
apply_properties = {
["node.description"] = "Audio Jack",
},
}
rename_hdmi_audio_rule = {
matches = {
{
{"api.alsa.card.name", "equals", "Starfive-HDMI-Sound-Card"},
},
},
apply_properties = {
["node.description"] = "HDMI Audio",
},
}
rename_wm8960_rule = {
matches = {
{
{"api.alsa.card.name", "equals", "Starfive-WM8960-Sound-Card"},
},
},
apply_properties = {
["node.description"] = "WM8960 Audio",
},
}
table.insert(alsa_monitor.rules, rename_wm8960_rule)
table.insert(alsa_monitor.rules, rename_audiojack_rule)
table.insert(alsa_monitor.rules, rename_hdmi_audio_rule)
