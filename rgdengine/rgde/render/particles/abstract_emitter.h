#pragma once

#include <rgde/render/particles/emitter.h>
#include <rgde/math/random.h>
#include <rgde/math/interpolyator.h>


namespace particles
{

class processor;
struct particle;


class  base_emitter : public emitter
{
public:
	typedef std::list<processor*> processors_list;
	typedef processors_list::iterator processors_iter;

	base_emitter(emitter::Type);
	virtual ~base_emitter();

	virtual void		get_particle(particle& p);

	void				reset();
	void				update(float dt);
	void				render();
	virtual void		debug_draw() = 0;

	void				addProcessor(processor*	 proc);	
	void				deleteProcessor(processor* proc);	
	
protected:
	virtual void to_stream(io::write_stream& wf) const;
	virtual void from_stream(io::read_stream& rf);

// ����������
public:
	inline processors_list& getProcessors() { return m_lProcessors; }

	inline float getTime() { return m_fTimeNormalaized; }
	inline math::vec3f&	getSpeed() {return m_vCurSpeed;}

	// ���������� �������
	void setFade(bool b);

	// ���������� � ��������������
	inline math::FloatInterp& particleMass()			{ return m_PMass; }
	inline math::FloatInterp& particleMassSpread()		{ return m_PMassSpread; }
	inline math::FloatInterp& particleRotationSpread()	{ return m_PRotationSpread; }
	inline math::FloatInterp& particleVelocity()		{ return m_velocity; }
	inline math::FloatInterp& particleVelocitySpread()	{ return m_PVelSpread; }
	inline math::Vec3Interp& particleAcceleration()		{ return m_PAcceleration; }
	inline math::Vec3Interp& getGlobalVelocityInterp()	{ return m_GlobalVelocity; }

	// ���������� ��������� / ������� �������
	inline float getCycleTime() const { return m_fCycleTime; }
	inline void setCycleTime(float fTime) { m_fCycleTime = fTime; }

	inline bool isCycling() const { return m_bIsCycling; }
	inline void setCycling(bool b) { m_bIsCycling = b; }

	inline bool	isVisible() const {return m_is_visible;}
	inline void	hide() { m_is_visible = false; }
	inline void	show() { m_is_visible = true; }

	inline float getTimeShift() const { return m_time_shift; }
	inline void setTimeShift(float t) { m_time_shift = t; }


protected:
	math::unit_rand_2k	m_Rand;

	processors_list	m_lProcessors;				// �������������� ���������� ������

	float			m_fCycleTime;				// ����� ������� ��� ���� ��������������
	bool			m_bIsCycling;
	bool			m_is_visible;
	float			m_time_shift;				// �������� � �������� �� ������ ������������ �������
	std::string		m_name;					// ��� �������� �������������

	// common for all emmiters types modifiers
	math::FloatInterp	m_PMass;				// ����� �������
	math::FloatInterp	m_PMassSpread;			// ������������ ������� �� ����� ������
	math::FloatInterp	m_PRotationSpread;		// ������� ��������
	math::FloatInterp	m_velocity;			// �������� ������
	math::FloatInterp	m_PVelSpread;			// ������� ��������
	math::Vec3Interp	m_PAcceleration;		// ��������� �������
	math::Vec3Interp	m_GlobalVelocity;		// ���������� �������� ��������
	
	// temporary computing values
	float			m_fTimeNormalaized;			// �� 0 �� 1 - ������� ��������������� �����
	float			m_fCurrentTime;				// ������� ����� (������ ������� �������)
	bool			m_bIsEnded;					// ����: ������� ���������

	math::vec3f		m_vCurSpeed;
	math::vec3f		m_vCurSpeedTransformed;
	math::vec3f		m_vCurDisplacement;
	math::vec3f		m_vOldPos;

	math::vec3f		m_vAccelerationPrecomputed;
	math::vec3f		m_vPAcceleration;
	math::vec3f		m_vGlobalVelPrecomputed;
	math::vec3f		m_vGlobalVel;
};

}