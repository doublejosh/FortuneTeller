import React from 'react'

export default f => {
	const isDesktop = () => {
		let desktopQuery = window.matchMedia('(min-width: 600px)')
		return desktopQuery.matches
	}

	const minWidth = isDesktop() ? 20 : 10
	const minScore = 10
	const width = f.score > minScore ? f.score : minWidth
	const classes = f.score < minScore ? ' low-score' : ''

	return (
		<li>
			<p>{f.text}</p>
			<div
				style={{ width: width + '%' }}
				className={'score-badge' + classes}
			>
				<span className="number">{f.score}%</span>
			</div>
			<div className="data">
				{f.votes} votes / {f.shown} views
			</div>
		</li>
	)
}
