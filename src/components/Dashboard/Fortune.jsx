import React from 'react'
import { css } from 'glamor'

export default ({ f, hideVotes }) => {
	return (
		<li
			{...css({
				padding: '0.5rem 0 0.5rem 0.5rem',
				marginBottom: '0.5rem',
				boxShadow: '-3px 3px 0px rgba(0, 0, 0, 0.4)',
				background: 'rgba(255, 255, 255, 0.1)',
				position: 'relative',
				listStyleType: 'none',
				'@media (min-width: 600px)': {
					padding: '0.75rem 0 0.75rem 0.75rem',
					marginBottom: '1rem',
				},
				opacity: (f.score / 100) * 0.5 + 0.5,
			})}>
			<p
				{...css({
					position: 'relative',
					zIndex: '10',
					margin: '0',
					fontSize: '1.5rem',
					paddingRight: '4rem',
					'@media (min-width: 600px)': {
						paddingRight: '235px',
					},
				})}>
				{f.text}
				<span {...css({ marginLeft: '.5rem', fontSize: '1rem', opacity: 0.5 })}>
					({f.rank && f.rank.total ? f.rank.total : 0})
				</span>
			</p>
			<div
				style={{ width: f.score > 10 ? f.score + '%' : '3.25rem' }}
				{...css({
					position: 'absolute',
					right: '0',
					top: '0',
					height: '100%',
					background: `linear-gradient(90deg,
								rgba(36, 58, 158, 0.5) 0%,
								rgb(36, 58, 158) 100%)`,
					textAlign: 'right',
					display: 'flex',
					alignItems: 'center',
					justifyContent: 'flex-end',
				})}>
				<span
					{...css({
						color: '#000',
						fontSize: '1.5rem',
						padding: '0 1rem',
					})}>
					{f.score}%
				</span>
			</div>
			{!hideVotes && (
				<div
					{...css({
						marginTop: '0.5rem',
						fontSize: '1rem',
						fontWeight: '100',
						color: '#000',
					})}>
					{f.votes} votes / {f.shown} views
				</div>
			)}
		</li>
	)
}
