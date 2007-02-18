#pragma once

#include "render/particles/particlesEmitter.h"
#include "math/mathRandom.h"
#include "math/mathInterpolyator.h"


namespace particles
{

class IParticlesProcessor;
struct Particle;


class  IAbstractEmitter : public IEmitter
{
public:
	typedef std::list<IParticlesProcessor*> tPProcessors;
	typedef tPProcessors::iterator tPProcessorsIter;

	IAbstractEmitter(IEmitter::Type);
	virtual ~IAbstractEmitter();

	virtual void		getParticle(Particle& p);

	void				reset();
	void				update(float dt);
	void				render();
	virtual void		debugDraw() = 0;

	void				addProcessor(IParticlesProcessor*	 pProcessor);	
	void				deleteProcessor(IParticlesProcessor* pProcessor);	
	
protected:
	virtual void toStream(io::IWriteStream& wf) const;
	virtual void fromStream(io::IReadStream& rf);

// ����������
public:
	inline tPProcessors& getProcessors() { return m_lProcessors; }

	inline float getTime() { return m_fTimeNormalaized; }
	inline math::Vec3f&	getSpeed() {return m_vCurSpeed;}

	// ���������� �������
	void setFade(bool b);

	// ���������� � ��������������
	inline math::FloatInterp& particleMass()			{ return m_PMass; }
	inline math::FloatInterp& particleMassSpread()		{ return m_PMassSpread; }
	inline math::FloatInterp& particleRotationSpread()	{ return m_PRotationSpread; }
	inline math::FloatInterp& particleVelocity()		{ return m_PVelocity; }
	inline math::FloatInterp& particleVelocitySpread()	{ return m_PVelSpread; }
	inline math::Vec3Interp& particleAcceleration()		{ return m_PAcceleration; }
	inline math::Vec3Interp& getGlobalVelocityInterp()	{ return m_GlobalVelocity; }

	// ���������� ��������� / ������� �������
	inline float getCycleTime() const { return m_fCycleTime; }
	inline void setCycleTime(float fTime) { m_fCycleTime = fTime; }

	inline bool isCycling() const { return m_bIsCycling; }
	inline void setCycling(bool b) { m_bIsCycling = b; }

	inline bool	isVisible() const {return m_bIsVisible;}
	inline void	hide() { m_bIsVisible = false; }
	inline void	show() { m_bIsVisible = true; }

	inline float getTimeShift() const { return m_fTimeShift; }
	inline void setTimeShift(float t) { m_fTimeShift = t; }


protected:
	math::UnitRandom2k	m_Rand;

	tPProcessors	m_lProcessors;				// �������������� ���������� ������

	float			m_fCycleTime;				// ����� ������� ��� ���� ��������������
	bool			m_bIsCycling;
	bool			m_bIsVisible;
	float			m_fTimeShift;				// �������� � �������� �� ������ ������������ �������
	std::string		m_strName;					// ��� �������� �������������

	// common for all emmiters types modifiers
	math::FloatInterp	m_PMass;				// ����� �������
	math::FloatInterp	m_PMassSpread;			// ������������ ������� �� ����� ������
	math::FloatInterp	m_PRotationSpread;		// ������� ��������
	math::FloatInterp	m_PVelocity;			// �������� ������
	math::FloatInterp	m_PVelSpread;			// ������� ��������
	math::Vec3Interp	m_PAcceleration;		// ��������� �������
	math::Vec3Interp	m_GlobalVelocity;		// ���������� �������� ��������
	
	// temporary computing values
	float			m_fTimeNormalaized;			// �� 0 �� 1 - ������� ��������������� �����
	float			m_fCurrentTime;				// ������� ����� (������ ������� �������)
	bool			m_bIsEnded;					// ����: ������� ���������

	math::Vec3f		m_vCurSpeed;
	math::Vec3f		m_vCurSpeedTransformed;
	math::Vec3f		m_vCurDisplacement;
	math::Vec3f		m_vOldPos;

	math::Vec3f		m_vAccelerationPrecomputed;
	math::Vec3f		m_vPAcceleration;
	math::Vec3f		m_vGlobalVelPrecomputed;
	math::Vec3f		m_vGlobalVel;
};

}