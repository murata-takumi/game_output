#pragma once
#include "FBX/AnimNodes/RUN00_FNode.h"
#include "FBX/FBXActor.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="actor">�|�C���^</param>
/// <param name="anim">�񋓑�</param>
RUN00_FNode::RUN00_FNode(FBXActor* actor, AnimEnum anim)
	:AnimNode(actor, anim)
{

}