#ifndef SECTION_HPP
# define SECTION_HPP

enum Section {
    REQUEST_LINE,
    HEADERS,
    SPECIAL_HEADERS,
    BODY,
    CHUNK,
    COMPLETE,
    ERROR
};

typedef struct s_client
		{
			int body_offset_;
			size_t chunk_size_;
			bool isChunked_;
			Section buffer_section_;
			timeval start_tv_;
		} t_client;

#endif