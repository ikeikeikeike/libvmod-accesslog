#include <stdlib.h>

#include "vrt.h"
#include "vrt_obj.h"
#include "vcc_if.h"
#include "bin/varnishd/cache.h"

#define vcl_string char

int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
	return (0);
}

const char *
vmod_to_json(struct sess *sp, const char *url, const char *referer, const char *user_agent)
{
	char *p;
	unsigned u, v;

	u = WS_Reserve(sp->wrk->ws, 0); /* Reserve some work space */
	p = sp->wrk->ws->f;		/* Front of workspace area */
	v = snprintf(p, u, 
		"{\"RemoteHost\": \"%s\", \"Time\": \"%s\", \"Method\": \"%s\", \"Status\": %d, \"Url\": \"%s\", \"Referer\": \"%s\", \"UserAgent\": \"%s\"}", 
		VRT_IP_string(sp, VRT_r_client_ip(sp)),
		VRT_time_string(sp, sp->t_req), 
		VRT_r_req_request(sp), 
		VRT_r_resp_status(sp), 
		url,
		referer, 
		user_agent
	);

	v++;
	if (v > u) {
		/* No space, reset and leave */
		WS_Release(sp->wrk->ws, 0);
		return (NULL);
	}
	/* Update work space with what we've used */
	WS_Release(sp->wrk->ws, v);
	return (p);
}
