#pragma once

#include "SerializableObject.h"
#include "../Types.h"

#define RPCName(inID) \
enum { k_RPCName = inID };

namespace RPCParams
{
	// ID: DCUS
	class UserDisconnedtRPC : public SerializableObject
	{
	public:
		RPCName('DCUS')

		virtual void Write(OutputMemoryBitStream& output) 
		{
			output.Write(m_DisconnectedUser);
		}
		virtual void Read(InputMemoryBitStream& input)
		{
			input.Read(m_DisconnectedUser);
		}

		UserDisconnedtRPC(UInt32 user) : m_DisconnectedUser(user) {}
		const UInt32 GetUser() const { return m_DisconnectedUser; }

	private:
		UInt32 m_DisconnectedUser;
	};

	// ID: DNGO
	class DestroyNetworkedGameObject : public SerializableObject
	{
	public:
		RPCName('DNGO')

		virtual void Write(OutputMemoryBitStream& output)
		{
			output.Write(m_DestroyedObject);
		}
		virtual void Read(InputMemoryBitStream& input)
		{
			input.Read(m_DestroyedObject);
		}

		DestroyNetworkedGameObject(UInt32 obj) : m_DestroyedObject(obj) {}
		const UInt32 GetDestroyedObject() const { return m_DestroyedObject; }

	private:
		UInt32 m_DestroyedObject;
	};

	// ID: SNGA
	class SetNetworkedGameObjectActive : public SerializableObject
	{
	public:
		RPCName('SNGA')

		virtual void Write(OutputMemoryBitStream& output)
		{
			output.Write(m_NetworkObject);
			output.Write(m_Active);
		}
		virtual void Read(InputMemoryBitStream& input)
		{
			input.Read(m_NetworkObject);
			input.Read(m_Active);
		}

		SetNetworkedGameObjectActive(UInt32 objID, bool active) : m_NetworkObject(objID), m_Active(active) {}
		SetNetworkedGameObjectActive() : m_NetworkObject(0U), m_Active(false) { }
		
		const UInt32 GetNetworkedObject() const { return m_NetworkObject; }
		const bool GetActiveStatus() const { return m_Active; }

	private:
		UInt32 m_NetworkObject;
		bool m_Active;
	};
}