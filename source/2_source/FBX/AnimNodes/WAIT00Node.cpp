#pragma once
#include "FBX/AnimNodes/WAIT00Node.h"
#include "FBX/FBXActor.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="actor">�|�C���^</param>
/// <param name="anim">�񋓑�</param>
WAIT00Node::WAIT00Node(FBXActor* actor, AnimEnum anim)
	:AnimNode(actor, anim)
{

}