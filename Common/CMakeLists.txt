set(SRCS	Src/Crypto.cpp
			Src/Crypto/SHA256.cpp
			Src/Map.cpp
			Src/Players.cpp
			Src/RawReader.cpp
			Src/Socket.cpp
			Src/Tiles.cpp
			)

add_library(common ${SRCS})
