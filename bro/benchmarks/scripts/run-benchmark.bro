
# Disable the separate DNS timeout as the pac2 analyzer doesn't have. Instead
# reduce the generic UDP timeout to the corresponding value.
redef dns_session_timeout = 0 secs;
redef udp_inactivity_timeout = 10secs;
