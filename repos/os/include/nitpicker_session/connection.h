/*
 * \brief  Connection to Nitpicker service
 * \author Norman Feske
 * \date   2008-08-22
 */

/*
 * Copyright (C) 2008-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__NITPICKER_SESSION__CONNECTION_H_
#define _INCLUDE__NITPICKER_SESSION__CONNECTION_H_

#include <nitpicker_session/client.h>
#include <framebuffer_session/client.h>
#include <input_session/client.h>
#include <util/arg_string.h>
#include <base/connection.h>

namespace Nitpicker { class Connection; }


class Nitpicker::Connection : public Genode::Connection<Session>,
                              public Session_client
{
	private:

		Framebuffer::Session_client _framebuffer;
		Input::Session_client       _input;

		/**
		 * Create session and return typed session capability
		 */
		Session_capability _connect(char const *label)
		{
			enum { ARGBUF_SIZE = 128 };
			char argbuf[ARGBUF_SIZE];
			argbuf[0] = 0;

			if (Genode::strlen(label) > 0)
				Genode::snprintf(argbuf, sizeof(argbuf), "label=\"%s\"", label);

			/*
			 * Declare ram-quota donation
			 */
			using Genode::Arg_string;
			enum { SESSION_METADATA = 36*1024 };
			Arg_string::set_arg(argbuf, sizeof(argbuf), "ram_quota", SESSION_METADATA);

			return session(argbuf);
		}

	public:

		/**
		 * Constructor
		 */
		Connection(char const *label = "")
		:
			/* establish nitpicker session */
			Genode::Connection<Session>(_connect(label)),
			Session_client(cap()),

			/* request frame-buffer and input sub sessions */
			_framebuffer(framebuffer_session()),
			_input(input_session())
		{ }

		void buffer(Framebuffer::Mode mode, bool use_alpha)
		{
			enum { ARGBUF_SIZE = 128 };
			char argbuf[ARGBUF_SIZE];
			argbuf[0] = 0;

			Genode::Arg_string::set_arg(argbuf, sizeof(argbuf), "ram_quota",
			                            ram_quota(mode, use_alpha));

			Genode::env()->parent()->upgrade(cap(), argbuf);
			Session_client::buffer(mode, use_alpha);
		}

		/**
		 * Return sub session for Nitpicker's input service
		 */
		Input::Session *input() { return &_input; }

		/**
		 * Return sub session for session's frame buffer
		 */
		Framebuffer::Session *framebuffer() { return &_framebuffer; }
};

#endif /* _INCLUDE__NITPICKER_SESSION__CONNECTION_H_ */
