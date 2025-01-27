#pragma newdecls required
#pragma semicolon 1

stock void ScaleVectors(const float vecA[3], const float vecB[3], float vecResult[3])
{
	vecResult[0] = vecA[0] * vecB[0];
	vecResult[1] = vecA[1] * vecB[1];
	vecResult[2] = vecA[2] * vecB[2];
}

stock void MoveVector(const float vecStart[3], const float vecEnd[3], float scale, float vecResult[3])
{
	SubtractVectors(vecEnd, vecStart, vecResult);
	ScaleVector(vecResult, scale);
	AddVectors(vecStart, vecResult, vecResult);
}

stock void GetVectorDistanceVector(const float vecA[3], const float vecB[3], float vecResult[3])
{
	SubtractVectors(vecA, vecB, vecResult);
	vecResult[0] = FloatAbs(vecResult[0]);
	vecResult[1] = FloatAbs(vecResult[1]);
	vecResult[2] = FloatAbs(vecResult[2]);
}

stock bool VecEqual(const float vecA[3], const float vecB[3])
{
	return (vecA[0] == vecB[0] && vecA[1] == vecB[1] && vecA[2] == vecB[2]);
}

// returns true if there's an intersection between two boxes
// borrowed from public/collisionutils.cpp
stock bool IsBoxIntersectingBox(const float boxMin1[3], const float boxMax1[3], const float boxMin2[3], const float boxMax2[3])
{
	if ((boxMin1[0] > boxMax2[0]) || (boxMax1[0] < boxMin2[0]))
		return false;
	if ((boxMin1[1] > boxMax2[1]) || (boxMax1[1] < boxMin2[1]))
		return false;
	if ((boxMin1[2] > boxMax2[2]) || (boxMax1[2] < boxMin2[2]))
		return false;
	return true;
}

stock any Min(any val1, any val2)
{
	return val1 < val2 ? val1 : val2;
}

stock any Max(any val1, any val2)
{
	return val1 > val2 ? val1 : val2;
}

stock any Clamp(any val, any min, any max)
{
	if (val < min)
		val = min;
	else if (val > max)
		val = max;
	return val;
}

stock bool IsInBounds(any value, any min, any max)
{
	if (value < min || value > max)
		return false;
	
	return true;
}
